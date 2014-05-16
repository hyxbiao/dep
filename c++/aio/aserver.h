

#ifdef	__ASERVER_H
#define	__ASERVER_H

#include "watcher.h"
#include "reactor.h"

#define	BACKLOG	50
#define	LOCAL_IP	"127.0.0.1"
#define	PORT	8701

class AsyncServer
{
public:
	AsyncServer();

	void set_reactor(Reactor *reactor);

	int init();

	void run();

private:
	int	_backlog;
	int	_port;

	Reactor *_reactor;
	int _accept_fd;
};

#endif	//__ASERVER_H
