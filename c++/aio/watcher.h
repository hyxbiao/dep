
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
	void (*cb)(Reactor *, Watcher *, int),
	int fd,
	int event
} Watcher;

/*
class Watcher
{
public:
	typedef void (*cb_t)(Watcher *, int);
public:
	Watcher();

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
