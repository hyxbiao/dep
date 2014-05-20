
#include "common.h"
#include "utils.h"
#include "aserver.h"


AsyncServer::AsyncServer(int backlog, int port, int conn_pool_size)
	:_backlog(backlog), _port(port)
	 , _conn_pool_size(conn_pool_size), _conn_pool(NULL)
	 , _asocket(NULL)
{
}

int AsyncServer::init()
{
	const int on = 1;
	_accept_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_accept_fd == -1) {
		LOG_E("create socket fail !");
		return -1;
	}

	setsockopt(_accept_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	setnonblocking(_accept_fd);

	struct sockaddr_in my_addr;
	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	//my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//inet_aton(LOCAL_IP, &(my_addr.sin_addr));
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_port = htons(_port);

	if (bind(_accept_fd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
		LOG_E("bind fail!");
		return -1;
	}
	if (listen(_accept_fd, _backlog) == -1) {
		LOG_E("listen fail!");
		return -1;
	}

	return 0;
}

void AsyncServer::set_asocket(AsyncSocket *asocket)
{
	_asocket = asocket;
}

void AsyncServer::run(Reactor *reactor)
{
	//lazy init connection pool for multi processer
	_conn_pool = new ConnectionPool(_conn_pool_size);
	if (_conn_pool->init() < 0) {
		LOG_E("init connection pool fail");
		return;
	}

	//start accept
	accept(reactor);

	//loop run
	reactor->run();
}

int AsyncServer::accept(Reactor *reactor)
{
	watcher_t w;
	w.fd = _accept_fd;
	w.cb = this->accept_callback;
	w.event = EV_READ;

	w.data = this;

	return reactor->start(&w);
}

void AsyncServer::on_accept(connection_t *conn)
{
	int fd = conn->fd;

	setnonblocking(fd);

	conn->mem_pool = MemPool::create(MEM_POOL_ALIGNMENT, MEM_POOL_SIZE);
	//begin read
	_asocket->aread(conn);
}

static void AsyncServer::accept_callback(Reactor *reactor, watcher_t *w, int revent)
{
	struct sockaddr_un peer_addr;
	socklen_t peer_addr_size = sizeof(struct sockaddr_un);

	int accept_fd = w->fd;
	AsyncServer *aserver = (AsyncServer *)w->data;

	int clientfd = ::accept(accept_fd, (struct sockaddr *) &peer_addr, &peer_addr_size);
	if(clientfd < 0) {
		//finish
		if(errno != EAGAIN && errno != EWOULDBLOCK) {
			LOG_W("accept fail! error: %s", strerror());
		}
		return;
	}
	LOG_D("accept success");

	connection_t *conn = _conn_pool->get(clientfd);
	if (conn == NULL) {
		LOG_W("no free connection_t");
		return;
	}

	aserver->on_accept(conn);

	return;
}
