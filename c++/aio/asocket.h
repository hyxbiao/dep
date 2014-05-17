
#ifdef	__ASOCKET_H
#define	__ASOCKET_H

#include "watcher.h"
#include "reactor.h"

typedef struct
{
	int fd,
	void (*cb)(SocketEvent *, int event)
} SocketEvent;

class AsyncSocket
{
public:
	typedef	void (*cb_t)(AsyncSocket *, int event);
public:
	AsyncSocket(Reactor *reactor);

	int read(cb_t cb, int fd, void *buf, size_t count);

	int write(cb_t cb, int fd, const void *buf, size_t count);

protected:
	void read_callback(Reactor *reactor, watcher_t *w, int revent);

private:
	Reactor *_reactor;
};

#endif	//__ASOCKET_H
