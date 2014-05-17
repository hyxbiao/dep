

#ifdef	__ASERVER_H
#define	__ASERVER_H

#include "connpool.h"
#include "watcher.h"
#include "reactor.h"

#define	BACKLOG	50
#define	LOCAL_IP	"127.0.0.1"
#define	PORT	8701

class AsyncServer
{
public:
	AsyncServer(int backlog, int port, int conn_pool_size);

	int init();

	void run(Reactor *reactor);

protected:
	virtual void on_accept(connection_t *conn);

	virtual void accept_callback(Reactor *reactor, watcher_t *w, int revent);

private:
	int accept(Reactor *reactor);

private:
	int	_backlog;
	int	_port;

	int _accept_fd;

	ConnectionPool *_conn_pool;
};

#endif	//__ASERVER_H
