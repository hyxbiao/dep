
#ifdef	__WATCHER_H
#define	__WATCHER_H

enum {
	EV_NONE     =               0x00,
	EV_READ     =               0x01,
	EV_WRITE    =               0x10,
	EV_ERROR    =    (int)0x80000000
};

class Reactor;

typedef struct {
	int active,
	int fd,
	int event,
	void (*cb)(Reactor *, watcher_t *, int),
	void *data	/*custom*/
} watcher_t;

/*
class watcher_t
{
public:
	typedef void (*cb_t)(watcher_t *, int);
public:
	watcher_t();

	void init(cb_t cb, int fd, int event);

	void callback(int revent);

	void set_callback(cb_t cb);

	void set_event(int event);

	int get_event();

	int get_handle();

private:
	cb_t _cb;
	int _fd;
	int _event;
};
*/


#endif	//__WATCHER_H
