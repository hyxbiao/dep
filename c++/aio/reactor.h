
#ifdef	__REACTOR_H
#define	__REACTOR_H

#include "watcher.h"
#include "epollx.h"

class Reactor
{
public:
	Reactor();

	int init();

	void destroy();

	int start(watcher_t *w);

	int stop(watcher_t *w);

	void run();

private:
	Epollx *_epollx;
	int	_max_event;

	watcher_t *_wlist;
	int _watcher_pool_size;
};


#endif	//__REACTOR_H
