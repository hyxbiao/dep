
#include "watcher.h"


Watcher::Watcher()
	:_cb(NULL), _fd(-1), _event(EV_NONE)
{
}

void Watcher::init(cb_t cb, int fd, int event)
{
	_cb = cb;
	_fd = fd;
	_event = event;
}

void Watcher::callback(int revent)
{
	_cb(this, revent);
}

void Watcher::set_callback(cb_t cb)
{
	_cb = cb;
}

void Watcher::set_event(int event)
{
	_event = event;
}

int Watcher::get_event()
{
	return _event;
}

int Watcher::get_handle()
{
	return _fd;
}
