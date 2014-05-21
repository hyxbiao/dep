
#include "common.h"
#include "xheadsocket.h"


XHeadSocket::XHeadSocket(Reactor *reactor)
	:AsyncSocket(reactor)
{
}

int XHeadSocket::init(connection_t *conn)
{
	if (AsyncSocket::init(conn) != 0) {
		return -1;
	}

	conn->read_buf_size = 1024;
	conn->read_buf = conn->mem_pool->alloc(conn->read_buf_size);
	if (conn->read_buf == NULL) {
		LOG_W("alloc read buf fail");
		return -1;
	}

	return 0;
}

int XHeadSocket::aread(connection_t *conn)
{
	//read xhead_t
	conn->read_cb = read_xhead_cb;

	read(conn, sizeof(xhead_t));

	return 0;
}

int XHeadSocket::awrite(connection_t *conn)
{
	conn->write_cb = write_cb;

	xhead_t *res_head = (xhead_t *)conn->write_buf;

	size_t count = sizeof(xhead_t) + res_head->body_len;

	write(conn, count);

	return 0;
}

int XHeadSocket::finish(connection_t *conn)
{
	return AsyncSocket::finish(conn);
}

void XHeadSocket::read_xhead_cb(connection_t *conn)
{
	AsyncSocket *asocket = conn->asocket;

	if (conn->status & S_ERROR) {
		asocket->close(conn);
		return;
	}

	xhead_t *req_head = (xhead_t *)conn->read_buf;
	unsigned int body_len = req_head->body_len;

	size_t left = conn->read_buf_size - conn->read_cnt;
	if (body_len > left) {
		LOG_W("no enough buffer, body_len: %u, left: %lu", body_len, left);
		return;
	}

	conn->read_cb = XHeadSocket::read_xbody_cb;
	asocket->read(conn, body_len);
}

void XHeadSocket::read_xbody_cb(connection_t *conn)
{
	AsyncSocket *asocket = conn->asocket;

	if (conn->status & S_ERROR) {
		asocket->close(conn);
		return;
	}

	//xhead_t *req_head = (xhead_t *)conn->read_buf;
	//void *body = (void *)(req_head + 1);
	asocket->action(conn, ACTION_READ_DONE);
}

void XHeadSocket::write_cb(connection_t *conn)
{
	AsyncSocket *asocket = conn->asocket;

	if (conn->status & S_ERROR) {
		asocket->close(conn);
		return;
	}

	//xhead_t *req_head = (xhead_t *)conn->read_buf;
	//void *body = (void *)(req_head + 1);
	asocket->action(conn, ACTION_WRITE_DONE);

	asocket->finish(conn);
}
