
#ifndef	__CONNPOOL_H
#define	__CONNPOOL_H

#include "mempool.h"

class ConnectionPool;

typedef struct
{
	int fd,
	int active,
	int status,		//socket event status

	void *read_buf,
	size_t read_buf_size,

	size_t read_cnt,
	size_t need_read_cnt,

	void *write_buf,
	size_t write_buf_size,

	size_t write_cnt,
	size_t need_write_cnt,

	ConnectionPool *conn_pool,	// for free this anywhere
	void *asocket,
	MemPool *mem_pool,

	void *data
} connection_t;

//free pool模式在多线程下不安全，需要加锁
//以fd为index的array pool可以实现无锁，如果ConnectionPool只用于socket句柄，则会浪费部分空间（文件句柄）
class ConnectionPool
{
public:
	ConnectionPool(int conn_pool_size);

	int init();

	connection_t * get(int fd);
	//connection_t * get();
	
	void free(connection_t *conn);
private:
	int _pool_size;
	int _free_size;
	connection_t *_list;
	connection_t *_free_list;
};

#endif	//__CONNPOOL_H

