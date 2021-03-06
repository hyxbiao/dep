
#include <stdlib.h>

#include "common.h"
#include "epollx.h"


Epollx::Epollx(Reactor *reactor, int max_event) 
	:_max_event(max_event), _events(NULL), _reactor(reactor)
{
}

Epollx::~Epollx()
{
	destroy();
}


int Epollx::init() 
{
	_epollfd = epoll_create(256);
	if (_epollfd == -1) {
		return -1;
	}
	_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * _max_event);
	if (_events == NULL) {
		return -1;
	}
	return 0;
}

void Epollx::destroy() 
{
	if (_events != NULL) {
		free(_events);
		_events = NULL;
	}
}

int Epollx::add(watcher_t *w)
{
	struct epoll_event ev;

	int fd = w->fd;

	int wev = w->event;

	int events = (wev & EV_READ  ? EPOLLIN  : 0)
			   | (wev & EV_WRITE ? EPOLLOUT : 0);

	ev.events = events;
	ev.data.ptr = (void *)w;

	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		LOG_W("epoll add fail!");
		return -1;
	}
	return 0;
}

int Epollx::mod(watcher_t *w)
{
	struct epoll_event ev;

	int fd = w->fd;
	
	int wev = w->event;

	int events = (wev & EV_READ  ? EPOLLIN  : 0)
			   | (wev & EV_WRITE ? EPOLLOUT : 0);

	ev.events = events;
	ev.data.ptr = (void *)w;

	if (epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &ev) == -1) {
		LOG_W("epoll mod fail!");
		return -1;
	}
	return 0;
}

int Epollx::del(watcher_t *w)
{
	int fd = w->fd;
	
	if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		LOG_W("epoll del fail!");
		return -1;
	}
	return 0;
}

int Epollx::poll(int timeout)
{
	int nfds = epoll_wait(_epollfd, _events, _max_event, timeout);
	if (nfds < 0) {
		return -1;
	}
	for (int i=0; i<nfds; i++) {
		struct epoll_event *ev = _events + i;
		watcher_t *w = (watcher_t *)ev->data.ptr;
		int got = (ev->events & (EPOLLOUT)            ? EV_WRITE : 0) 
			    | (ev->events & (EPOLLIN)             ? EV_READ  : 0)
			    | (ev->events & (EPOLLERR | EPOLLHUP) ? EV_ERROR : 0);

		w->cb(_reactor, w, got);
	}
	return 0;
}

/*
int Epollx::wait(struct epoll_event *events, int maxevents, int timeout)
{
	int ret = epoll_wait(_epollfd, events, maxevents, timeout);
	return ret;
}
*/
