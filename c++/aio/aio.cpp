#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include "epoll.h"

#define	BACKLOG	50
#define	LOCAL_IP	"127.0.0.1"
#define	PORT	8701

#define	CHILDS	10
#define	MAX_EVENTS	100

#define	CPUS	4

int setnonblocking(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
	if(flags < 0 || fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0){ 
		printf("set nonblock fail!\n");
		return 1;
	}
	return 0;
}
int init(int &sockfd) 
{
	const int on = 1;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		printf("create socket fail !\n");
		return 1;
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	setnonblocking(sockfd);

	struct sockaddr_in my_addr;
	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	//my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	//inet_aton(LOCAL_IP, &(my_addr.sin_addr));
	my_addr.sin_port = htons(PORT);

	if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
		printf("bind fail!\n");
		perror("bind: ");
		return 1;
	}
	if(listen(sockfd, BACKLOG) == -1) {
		printf("listen fail!\n");
		return 1;
	}

	return 0;
}

int accept_handle(int pid, Epoll *epoll, int listenfd)
{
	struct sockaddr_un peer_addr;
	socklen_t peer_addr_size;
	peer_addr_size = sizeof(struct sockaddr_un);

	while(true) {
		printf("[child: %d] start accept\n", pid);
		int clientfd = accept(listenfd, (struct sockaddr *) &peer_addr, &peer_addr_size);
		if(clientfd == -1) {
			//finish
			if(errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			}
			printf("[child: %d] accept fail!\n", pid);
			perror("accept");
			return 1;
		}
		printf("[child: %d] accept success\n", pid);
		setnonblocking(clientfd);
		epoll->add(clientfd);
	}
	printf("[child: %d] done accept\n", pid);
	return 0;
}

int write_handle(int fd)
{
	char str[] = "welcome";
	send(fd, str, sizeof(str), 0);
	return 0;
}

int read_handle(int fd)
{
	ssize_t count = 0;
	ssize_t readed = 0;
	char buf[512];

	while(true) {
		count = read(fd, buf + readed, sizeof(buf) - readed);
		if(count == -1) {
			//errno == EAGAIN means we have read all data
			if(errno != EAGAIN) {
				perror("read");
				close(fd);
			}
			break;
		} else if(count == 0) {	/* remote close socket */
			printf("remote close");
			close(fd);
			break;
		}
		readed += count;
	}
	buf[readed] = 0;
	printf("read buf: %s\n", buf);

	write_handle(fd);
	return 0;
}

int start_worker(int cpu_idx, int listenfd)
{
	cpu_set_t mask;

	struct epoll_event events[MAX_EVENTS];

	int pid = getpid();

	CPU_ZERO(&mask);
	CPU_SET(cpu_idx, &mask);
	if(sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
		printf("[child: %d][cpu_idx: %d] bind cpu fail\n", pid, cpu_idx);
	}

	Epoll *epoll = new Epoll();
	epoll->init();
	epoll->add(listenfd);

	while(true) {
		int nfds = epoll->wait(events, MAX_EVENTS, -1);
		if(nfds == -1) {
			printf("[child: %d] epoll wait fail!\n", pid);
			delete epoll;
			return 1;
		}
		for(int i=0; i<nfds; i++) {
			if(events[i].events & EPOLLERR ||
				events[i].events & EPOLLHUP) {
				perror("epoll");
				printf("epoll error\n");
				close(events[i].data.fd);
				continue;
			} else if(events[i].data.fd == listenfd) {
				if(accept_handle(pid, epoll, listenfd) != 0) {
					delete epoll;
					return 1;
				}
			} else {
				read_handle(events[i].data.fd);
			}
		}
	}
	delete epoll;
	return 0;
}

int start_daemon()
{
	int status;
	while(true) {
		//int pid = waitpid(-1, &status, WNOHANG);
		int pid = waitpid(-1, &status, 0);
		printf("[parent] pid: %d\n", pid);
		if(pid <= 0) {
			return 0;
		}
	}
	return 0;
}

int main() 
{
	int listenfd = 0;

	//init
	if(init(listenfd) != 0) {
		return -1;
	}
	printf("listen fd : %d\n", listenfd);

	//fork
	for(int i=0; i<CHILDS; i++) {
		int cpu_idx = i % CPUS;
		int ret = fork();
		if(ret == -1) {
			printf("fork error!");
			return 0;
		} else if(ret == 0) {
			start_worker(cpu_idx, listenfd);
			return 0;
		}
	}
	
	//start daemon
	start_daemon();

	return 0;
}
