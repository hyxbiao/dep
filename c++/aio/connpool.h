
#ifndef	__CONNPOOL_H
#define	__CONNPOOL_H

typedef struct
{
	int fd,
	void *read_buf,
	int read_buf_size,

	void *write_buf,
	int write_buf_size,

	void *data
} connection_t;

class ConnectionPool
{
public:
	ConnectionPool(int conn_pool_size);

	int init();

	connection_t * get();
	void free(connection_t *conn);
private:
	int _pool_size;
	int _free_size;
	connection_t *_list;
	connection_t *_free_list;
};

#endif	//__CONNPOOL_H

