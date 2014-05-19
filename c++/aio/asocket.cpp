
#include "asocket.h"

AsyncSocket::AsyncSocket(Reactor *reactor, MemPool *mempool)
	:_reactor(reactor), _mempool(mempool)
{
}

int AsyncSocket::aread(connection_t *conn)
{
	conn->asocket = this;
	//TODO read data
	return 0;
}

int AsyncSocket::read(connection_t *conn, size_t count)
{
	int fd = conn->fd;

	if (conn->status & S_READABLE == 0) {
		LOG_W("socket is not readable");
		return -1;
	}
	if (conn->need_read_cnt + count > conn->read_buf_size) {
		LOG_W("no more buffer");
		return -1;
	}
	conn->need_read_cnt += count;

	watcher_t w;
	w.fd = fd;
	w.cb = AsyncSocket::read_callback;
	w.event = EV_READ;

	w.data = conn;

	int ret = _reactor->start(&w);
	if (ret < 0) {
		LOG_W("reactor start read fail");
		conn->need_read_cnt -= count;
		return -1;
	}
	return 0;
}

void AsyncSocket::on_read(connection_t *conn)
{
	int fd = conn->fd;
	int status = conn->status;

	watcher_t w;
	w.fd = fd;
	if (status == S_CLOSE) {
		_reactor->stop(&w);
		::close(fd);
		return;
	}
	//read all need_read_cnt data done
	if (conn->read_cnt == conn->need_read_cnt) {
		_reactor->stop(&w);
		_read_done_cb(conn, _read_done_cb_p);
	}
}

static void AsyncSocket::read_callback(Reactor *reactor, watcher_t *w, int revent)
{
	int fd = w->fd;

	connection_t *conn = w->data;

	AsyncSocket *asocket = conn->asocket;

	int ret = ::read(fd, conn->read_buf + conn->read_cnt, conn->need_read_cnt - conn->read_cnt);
	if (ret < 0) {
		//TODO error
		if (error == EAGAIN) {
		}
		conn->status |= S_ERROR;
	} else if (ret == 0) {
		//fd close
		conn->status = S_CLOSE;
	} else {
		conn->read_cnt += ret;
	}

	asocket->on_read(conn);
}
