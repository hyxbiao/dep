
#ifndef	__XHEADSOCKET_H
#define	__XHEADSOCKET_H

#include "connpool.h"
#include "asocket.h"

typedef struct _xhead_t
{
	int version;
	unsigned int body_len;
} xhead_t;

class XHeadSocket : public AsyncSocket
{
public:
	XHeadSocket(Reactor *reactor);

	virtual int init(connection_t *conn);

	virtual int aread(connection_t *conn);

	virtual int awrite(connection_t *conn);

	virtual int finish(connection_t *conn);

	/*
	virtual void set_read_done_cb(cb_t cb);
	virtual cb_t get_read_done_cb();

	virtual void set_write_done_cb(cb_t cb);
	virtual cb_t get_write_done_cb();
	*/

protected:
	static void read_xhead_cb(connection_t *conn);
	static void read_xbody_cb(connection_t *conn);
	static void write_cb(connection_t *conn);
};

#endif	//__XHEADSOCKET_H

