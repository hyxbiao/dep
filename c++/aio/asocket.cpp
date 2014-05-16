
#include "asocket.h"

AsyncSocket::AsyncSocket(Reactor *reactor)
	:_reactor(reactor)
{
}

int AsyncSocket::read(int fd, void *buf, size_t count)
{
	return 0;
}
