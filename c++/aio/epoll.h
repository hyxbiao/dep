
#include <stdio.h>
#include <sys/epoll.h>

class Epoll
{
public:
	Epoll();

	int init();

	int add(int fd, int events = EPOLLIN | EPOLLET);

	int del(int fd);

	int wait(struct epoll_event *events, int maxevents, int timeout);

private:
	int _epollfd;
};
