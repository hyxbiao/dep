
#ifdef	__EPOLLX_H
#define	__EPOLLX_H

#include <stdio.h>
#include <sys/epoll.h>

#include "watcher.h"

class Epollx
{
public:
	Epollx(int max_event);
	~Epollx();

	int init();

	void destroy();

	int add(Watcher *w);

	int del(Watcher *w);

	int poll(int timeout);

	//int wait(struct epoll_event *events, int maxevents, int timeout);

private:
	int _epollfd;
	int _max_event;
	struct epoll_event *_events;
};


#endif	//__EPOLLX_H
