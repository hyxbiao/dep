
#include <unistd.h>
#include <errno.h>

#include "common.h"
#include "asocket.h"

AsyncSocket::AsyncSocket(Reactor *reactor)
	:_reactor(reactor)
	 ,_read_done_cb(NULL), _write_done_cb(NULL)
{
}

int AsyncSocket::init(connection_t *conn)
{
	conn->mem_pool = MemPool::create(MEM_POOL_ALIGNMENT, MEM_POOL_SIZE);
	if (conn->mem_pool == NULL) {
		LOG_W("create mem pool fail");
		return -1;
	}
	conn->asocket = this;
	conn->status = S_READABLE;

	return 0;
}

int AsyncSocket::finish(connection_t *conn)
{
	if (conn->status == S_CLOSE || conn->status & S_ERROR) {
		close(conn);
		return 0;
	}

	//type 1: long connection
	if (conn->type == 1) {
		conn->mem_pool->reset();
		conn->conn_pool->reset(conn);
		conn->status = S_READABLE;
		aread(conn);
	}
	return 0;
}
void AsyncSocket::action(connection_t *conn, int type)
{
	if (type == ACTION_READ_DONE) {
		if (_read_done_cb != NULL) {
			_read_done_cb(conn, _read_done_cbp);
		}
	} else if (type == ACTION_WRITE_DONE) {
		if (_write_done_cb != NULL) {
			_write_done_cb(conn, _write_done_cbp);
		}
	}
}

void AsyncSocket::set_read_done_cb(cb_t cb, void *cbp)
{
	_read_done_cb = cb;
	_read_done_cbp = cbp;
}

void AsyncSocket::set_write_done_cb(cb_t cb, void *cbp)
{
	_write_done_cb = cb;
	_write_done_cbp = cbp;
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

int AsyncSocket::write(connection_t *conn, size_t count)
{
	int fd = conn->fd;

	if (conn->status & S_WRITEABLE == 0) {
		LOG_W("socket is not writeable");
		return -1;
	}

	conn->need_write_cnt += count;

	//try write for avoid epoll_wait
	int ret = ::write(fd, ((char *)conn->write_buf) + conn->write_cnt, conn->need_write_cnt - conn->write_cnt);
	if (ret < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			goto WRITE_END;
		}
		conn->status |= S_ERROR;
		on_write(conn);
		return 0;
	} else {
		conn->write_cnt += ret;
	}
	//ret >= 0 will post event for loop

WRITE_END:
	watcher_t w;
	w.fd = fd;
	w.cb = AsyncSocket::write_callback;
	w.event = EV_WRITE;

	w.data = conn;

	int r = _reactor->start(&w);
	if (r < 0) {
		LOG_W("reactor start write fail");
		//reset
		if (ret > 0) {
			conn->write_cnt -= ret;
		}
		conn->need_write_cnt -= count;
		return -1;
	}
	return 0;
}

int AsyncSocket::close(connection_t *conn)
{
	int fd = conn->fd;

	//free connection_t
	conn->conn_pool->free(conn);

	//free mem pool
	if (conn->mem_pool) {
		MemPool::destroy(conn->mem_pool);
		conn->mem_pool = NULL;
	}

	return ::close(fd);
}

void AsyncSocket::on_read(connection_t *conn)
{
	int fd = conn->fd;
	int status = conn->status;

	watcher_t w;
	w.fd = fd;

	//TODO timeout
	
	//close
	if (status == S_CLOSE) {
		_reactor->stop(&w);
		close(conn);
		return;
	}

	//error or read all need_read_cnt data done
	if (conn->status & S_ERROR || conn->read_cnt == conn->need_read_cnt) {
		_reactor->stop(&w);
		if (conn->read_cb) {
			conn->read_cb(conn);
		}
		return;
	}
}

void AsyncSocket::on_write(connection_t *conn)
{
	int fd = conn->fd;
	int status = conn->status;

	watcher_t w;
	w.fd = fd;

	//TODO timeout
	
	//close
	if (status == S_CLOSE) {
		_reactor->stop(&w);
		close(conn);
		return;
	}

	//error or write done
	if (conn->status & S_ERROR || conn->write_cnt == conn->need_write_cnt) {
		_reactor->stop(&w);
		if (conn->write_cb) {
			conn->write_cb(conn);
		}
		return;
	}
}

void AsyncSocket::read_callback(Reactor *reactor, watcher_t *w, int revent)
{
	int fd = w->fd;

	connection_t *conn = (connection_t *)w->data;

	AsyncSocket *asocket = conn->asocket;

	int ret = ::read(fd, ((char *)conn->read_buf) + conn->read_cnt, conn->need_read_cnt - conn->read_cnt);
	if (ret < 0) {
		if (errno != EAGAIN) {
			conn->status |= S_ERROR;
		}
	} else if (ret == 0) {
		//fd close
		conn->status = S_CLOSE;
	} else {
		conn->read_cnt += ret;
	}

	asocket->on_read(conn);
}

void AsyncSocket::write_callback(Reactor *reactor, watcher_t *w, int revent)
{
	int fd = w->fd;

	connection_t *conn = (connection_t *)w->data;

	AsyncSocket *asocket = conn->asocket;

	int ret = ::write(fd, ((char *)conn->write_buf) + conn->write_cnt, conn->need_write_cnt - conn->write_cnt);
	if (ret < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			conn->status |= S_ERROR;
		}
	} else {
		conn->write_cnt += ret;
	}

	asocket->on_write(conn);
}
