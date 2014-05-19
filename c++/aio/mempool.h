
#ifndef	__MEMPOOL_H
#define	__MEMPOOL_H

class MemPool
{
public:
	MemPool() {}

	inline void *malloc(size_t size) {
		return ::malloc(size);
	}
	
	inline void free(void *ptr) {
		::free(ptr);
	}
};

#endif	//__MEMPOOL_H

