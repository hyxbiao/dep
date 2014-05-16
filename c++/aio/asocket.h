
#ifdef	__ASOCKET_H
#define	__ASOCKET_H

#include "watcher.h"
#include "reactor.h"

class AsyncSocket
{
public:
	AsyncSocket(Reactor *reactor);

	int accept();

	int read(int fd, void *buf, size_t count);

	int write(int fd, const void *buf, size_t count);

private:
	Reactor *_reactor;
};

#endif	//__ASOCKET_H
