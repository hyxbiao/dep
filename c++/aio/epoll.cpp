#include "epoll.h"


Epoll::Epoll() 
{
}


int Epoll::init() 
{
	_epollfd = epoll_create(100);
	if(_epollfd == -1) {
		return -1;
	}
	return 0;
}

int Epoll::add(int fd, int events)
{
	struct epoll_event ev;
	//ev.events = EPOLLIN | EPOLLET;
	ev.events = events;
	ev.data.fd = fd;

	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		printf("epoll add fail!\n");
		return -1;
	}
	return 0;
}

int Epoll::del(int fd)
{
	if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		printf("epoll del fail!\n");
		perror("epoll_ctl");
		return -1;
	}
	return 0;
}

int Epoll::wait(struct epoll_event *events, int maxevents, int timeout)
{
	int ret = epoll_wait(_epollfd, events, maxevents, timeout);
	return ret;
}
