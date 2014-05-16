
#include "common.h"
#include "utils.h"
#include "aserver.h"


AsyncServer::AsyncServer(int backlog, int port)
	:_backlog(backlog), _port(port), _reactor(NULL)
{
}


void AsyncServer::set_reactor(Reactor *reactor)
{
	_reactor = reactor;
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

void AsyncServer::run()
{
}
