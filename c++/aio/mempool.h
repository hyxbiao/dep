
#ifndef	__MEMPOOL_H
#define	__MEMPOOL_H


class MemPool
{
public:
	MemPool(void *p, size_t size);
	~MemPool();

	void *alloc(size_t size);
	
	//void free(void *ptr);

	void *get_pointer();
public:
	static MemPool *create(size_t alignment, size_t size);

	static void destroy(MemPool *pool);
private:
	void *_p;
	size_t _size;
	u_char *_last;
	u_char *_end;
};


#endif	//__MEMPOOL_H

