
#include "asocket.h"

AsyncSocket::AsyncSocket(Reactor *reactor)
	:_reactor(reactor)
{
}

int AsyncSocket::read(int fd, void *buf, size_t count)
{
	watcher_t w;
	w.fd = fd;
	w.cb = this->read_callback;
	w.event = EV_READ;

	return _reactor->start(&w);
}

