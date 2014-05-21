
#ifndef	__ASOCKET_H
#define	__ASOCKET_H

#include "connpool.h"
#include "watcher.h"
#include "reactor.h"

#define	MEM_POOL_ALIGNMENT	16
#define	MEM_POOL_SIZE	4096

enum {
	S_NONE      =               0x00,
	S_READABLE  =               0x01,
	S_WRITEABLE =               0x10,
	S_CLOSE     =              0x100,
	S_TIMEOUT   =             0x1000,
	S_SIGNAL    =            0x10000,
	S_ERROR     =    (int)0x80000000
};

enum {
	ACTION_READ_DONE	=		0x00,
	ACTION_WRITE_DONE	=		0x10
};

class AsyncSocket
{
public:
	typedef	void (* cb_t)(connection_t *, void *);

public:
	AsyncSocket(Reactor *reactor);

	virtual int init(connection_t *conn);

	virtual int aread(connection_t *conn) = 0;

	virtual int awrite(connection_t *conn) = 0;

	virtual int finish(connection_t *conn);

	virtual void action(connection_t *conn, int type);

	virtual void set_read_done_cb(cb_t cb, void *cbp);

	virtual void set_write_done_cb(cb_t cb, void *cbp);

	int read(connection_t *conn, size_t count);

	int write(connection_t *conn, size_t count);

	int close(connection_t *conn);

protected:
	virtual void on_read(connection_t *conn);
	static void read_callback(Reactor *reactor, watcher_t *w, int revent);

	virtual void on_write(connection_t *conn);
	static void write_callback(Reactor *reactor, watcher_t *w, int revent);

protected:
	Reactor *_reactor;

	cb_t _read_done_cb;
	void *_read_done_cbp;

	cb_t _write_done_cb;
	void *_write_done_cbp;
};

#endif	//__ASOCKET_H
