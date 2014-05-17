
#ifdef	__EPOLLX_H
#define	__EPOLLX_H

#include <stdio.h>
#include <sys/epoll.h>

#include "watcher.h"

class Reactor;
class Epollx
{
public:
	Epollx(int max_event);
	~Epollx();

	int init();

	void destroy();

	int add(watcher_t *w);

	int mod(watcher_t *w);

	int del(watcher_t *w);

	int poll(int timeout);

	//int wait(struct epoll_event *events, int maxevents, int timeout);

private:
	int _epollfd;
	int _max_event;
	struct epoll_event *_events;

	Reactor *_reactor;
};


#endif	//__EPOLLX_H
