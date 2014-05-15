
#include "common.h"
#include "epoll.h"


Epollx::Epollx(int max_event) 
	:_max_event(max_event), _events(NULL)
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
	_events = malloc(sizeof(struct epoll_event) * _max_event);
	if (_events == NULL) {
		return -1;
	}
	return 0;
}

void Epollx::destroy() 
{
	if (_events != NULL) {
		free(events);
		events = NULL;
	}
}

int Epollx::add(Watcher *w)
{
	struct epoll_event ev;

	int fd = w->get_handle();

	int ev = w->get_event();

	int events = (ev & EV_READ  ? EPOLLIN  : 0)
			   | (ev & EV_WRITE ? EPOLLOUT : 0);

	ev.events = events;
	ev.data.fd = fd;
	ev.data.ptr = (void *)w;

	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		LOG_W("epoll add fail!");
		return -1;
	}
	return 0;
}

int Epollx::del(Watcher *w)
{
	int fd = w->get_handle();
	
	if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		LOG_W("epoll del fail!");
		return -1;
	}
	return 0;
}

int Epollx::poll(int timeout)
{
	int nfds = epoll_wait(_events, _max_event, timeout);
	if (nfds < 0) {
		return -1;
	}
	for (int i=0; i<nfds; i++) {
		struct epoll_event *ev = _events + i;
		Watcher *w = (Watcher *)ev->data.ptr;
		int got = (ev->events & (EPOLLOUT | EPOLLERR | EPOLLHUP) ? EV_WRITE : 0) 
			    | (ev->events & (EPOLLIN  | EPOLLERR | EPOLLHUP) ? EV_READ  : 0);

		w->callback(got);
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
