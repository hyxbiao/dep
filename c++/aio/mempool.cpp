
#include <new>

#include "common.h"
#include "mempool.h"


MemPool::MemPool(void *p, size_t size)
	:_p(p), _size(size)
{
	reset();
}

MemPool::~MemPool()
{
}

void * MemPool::alloc(size_t size) 
{
	u_char *m;
	if (_last + size <= _end) {
		m = _last;
		_last += size;
		return m;
	}
	return NULL;
}

void MemPool::reset()
{
	_last = (u_char *)_p + sizeof(MemPool);
	_end = (u_char *)_p + _size;
}

void * MemPool::get_pointer()
{
	return _p;
}

MemPool * MemPool::create(size_t alignment, size_t size)
{
	void *p;
	int err;

	err = posix_memalign(&p, alignment, size);
	if (err) {
		LOG_E("posix_memalign(%luz, %luz) fail", alignment, size);
		return NULL;
	}
	MemPool *pool = new (p) MemPool(p, size);
	return pool;
}

void MemPool::destroy(MemPool *pool)
{
	if (pool != NULL) {
		void *p = pool->get_pointer();
		pool->~MemPool();
		::free(p);
	}
}
