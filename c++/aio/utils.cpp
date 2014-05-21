
#include <fcntl.h>
#include <unistd.h>

#include "common.h"
#include "utils.h"

int setnonblocking(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
	if(flags < 0 || fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0){ 
		LOG_W("set nonblock fail!");
		return -1;
	}
	return 0;
}
