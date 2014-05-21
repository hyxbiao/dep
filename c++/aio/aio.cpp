#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>

#include "common.h"
#include "mempool.h"
#include "xheadsocket.h"
#include "aserver.h"

#define	CHILDS	4
#define	MAX_EVENTS	64

#define	CPUS	4

void read_done_callback(connection_t *conn)
{
	AsyncSocket *asocket = conn->asocket;

	xhead_t *req_head = (xhead_t *)conn->read_buf;

	char *req_body = (char *)(req_head + 1);

	LOG_D("request body: %s", req_body);

	MemPool *pool = conn->mem_pool;
	conn->write_buf_size = 256;
	conn->write_buf = pool->alloc(conn->write_buf_size);
	
	xhead_t *res_head = (xhead_t *)conn->write_buf;
	char *res_body = (char *)(res_head + 1);
	size_t buf_size = conn->write_buf_size - sizeof(xhead_t);
	res_head->version = req_head->version;
	res_head->body_len = snprintf(res_body, buf_size, "hello, world!");

	asocket->awrite(conn);
}

int start_worker(int cpu_idx, AsyncServer *aserver)
{
	cpu_set_t mask;

	int pid = getpid();

	CPU_ZERO(&mask);
	CPU_SET(cpu_idx, &mask);
	if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
		LOG_D("[child: %d][cpu_idx: %d] bind cpu fail\n", pid, cpu_idx);
	}

	int max_events = 64;
	int pool_size = 1024;
	Reactor *reactor = new Reactor(max_events, pool_size);
	if (reactor->init() != 0) {
		LOG_E("reactor init fail");
		return -1;
	}
	AsyncSocket *asocket = new XHeadSocket(reactor);
	asocket->set_read_done_cb(read_done_callback);

	aserver->set_asocket(asocket);

	aserver->run(reactor);

	return 0;
}

int start_daemon()
{
	int status;
	while(true) {
		//int pid = waitpid(-1, &status, WNOHANG);
		int pid = waitpid(-1, &status, 0);
		LOG_D("[parent] pid: %d", pid);
		if(pid <= 0) {
			return 0;
		}
	}
	return 0;
}

int main() 
{
	int backlog = 50;
	int port = 8701;
	int conn_pool_size = 1024;

	//init
	AsyncServer *aserver = new AsyncServer(backlog, port, conn_pool_size);
	if (aserver->init() != 0) {
		LOG_E("init async server fail");
		return -1;
	}

	//fork
	for(int i=0; i<CHILDS; i++) {
		int cpu_idx = i % CPUS;
		int ret = fork();
		if(ret == -1) {
			LOG_E("fork error!");
			return 0;
		} else if(ret == 0) {
			start_worker(cpu_idx, aserver);
			return 0;
		}
	}
	
	//start daemon
	start_daemon();

	return 0;
}
