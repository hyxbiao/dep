
#ifndef	__CONNPOOL_H
#define	__CONNPOOL_H

typedef struct
{
	int fd,
	int active,
	int status,		//socket event status

	void *read_buf,
	int read_buf_size,

	int read_cnt,
	int need_read_cnt,

	void *write_buf,
	int write_buf_size,

	int write_cnt,
	int need_write_cnt,

	void *conn_pool,	// for free this anywhere
	void *asocket,

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

