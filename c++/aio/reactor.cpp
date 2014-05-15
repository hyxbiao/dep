
#include "reactor.h"


Reactor::Reactor()
	:_epollx(NULL)
{
}

int Reactor::init()
{
	_epollx = new Epollx();
	return _epollx->init();
}

void Reactor::destroy()
{
	if (_epollx != NULL) {
		delete _epollx;
		_epollx = NULL;
	}
}
