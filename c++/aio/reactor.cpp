
#include "reactor.h"


Reactor::Reactor(int max_event, int watcher_pool_size)
	:_epollx(NULL), _max_event(max_event)
	 , _whead(NULL), _watcher_pool_size(watcher_pool_size)
{
}

int Reactor::init()
{
	_wlist = malloc(_watcher_pool_size * sizeof(watcher_t));
	if (_wlist == NULL) {
		LOG_E("malloc watcher pool fail");
		return -1;
	}
	for (int i=0; i<_watcher_pool_size; i++) {
		watcher_t *w = _wlist + i;
		w->active = 0;
		w->fd = i;
	}

	_epollx = new Epollx(this, _max_event);
	return _epollx->init();
}

void Reactor::destroy()
{
	if (_epollx != NULL) {
		delete _epollx;
		_epollx = NULL;
	}
}

int Reactor::start(watcher_t *w)
{
	int fd = w->fd;
	if (fd < 0 || fd >= _watcher_pool_size) {
		LOG_W("fd is not valid");
		return -1;
	} 
	int ret = 0;
	watcher_t *_w = _wlist + fd;
	if (_w->active == 1) {
		//no need to modify if only cb change
		if (_w->event != w->event) {
			ret = _epollx->mod(w);
		}
	} else {
		ret = _epollx->add(w);
		_w->active = 1;
	}
	_w->cb = w->cb;
	_w->event = w->event;
	return ret;
}

int Reactor::stop(watcher_t *w)
{
	int fd = w->fd;
	if (fd < 0 || fd >= _watcher_pool_size) {
		LOG_W("fd is not valid");
		return -1;
	} 
	watcher_t *_w = _wlist + fd;
	if (_w->active == 1) {
		_w->active = 0;
		return _epollx->del(w);
	}
	return 0;
}

void Reactor::run()
{
	int timeout = -1;
	while(true) {
		_epollx->poll(-1);
	}
}
