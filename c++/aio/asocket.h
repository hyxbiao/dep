
#ifdef	__ASOCKET_H
#define	__ASOCKET_H

#include "connpool.h"
#include "watcher.h"
#include "reactor.h"

enum {
	S_NONE      =               0x00,
	S_READABLE  =               0x01,
	S_WRITEABLE =               0x10,
	S_CLOSE     =              0x100,
	S_TIMEOUT   =             0x1000,
	S_SIGNAL    =            0x10000,
	S_ERROR     =    (int)0x80000000
};

class AsyncSocket
{
public:
	typedef	void (*cb_t)(connection_t *, void *param);
public:
	AsyncSocket(Reactor *reactor);

	void set_read_done_callback(cb_t cb, void *param);

	virtual int aread(connection_t *conn);

	int read(connection_t *conn, size_t count);

	int write(connection_t *conn, size_t count);

	int close(connection_t *conn);

protected:
	virtual void on_read(connection_t *conn);
	static void read_callback(Reactor *reactor, watcher_t *w, int revent);

private:
	Reactor *_reactor;

	cb_t _read_done_cb;
	void* _read_done_cb_p;
	cb_t _write_done_cb;
	void* _write_done_cb_p;
};

#endif	//__ASOCKET_H
