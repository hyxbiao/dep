

#ifdef	__ASERVER_H
#define	__ASERVER_H

#include "connpool.h"
#include "asocket.h"
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

	void set_asocket(AsyncSocket *asocket);

	void run(Reactor *reactor);

protected:
	virtual void on_accept(connection_t *conn);

	static void accept_callback(Reactor *reactor, watcher_t *w, int revent);

private:
	int accept(Reactor *reactor);

private:
	int	_backlog;
	int	_port;

	int _accept_fd;

	ConnectionPool *_conn_pool;
	AsyncSocket *_asocket;
};

#endif	//__ASERVER_H
