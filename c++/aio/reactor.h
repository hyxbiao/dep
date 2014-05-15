
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

	int add(Watcher *w);

	void del(Watcher *w);

private:
	Epollx *_epollx;
};


#endif	//__REACTOR_H
