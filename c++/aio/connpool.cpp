
#include "connpool.h"

ConnectionPool::ConnectionPool(int pool_size)
	:_pool_size(pool_size), _list(NULL)
{
}

int ConnectionPool::init()
{
	_list = (connection_t *)malloc(_pool_size * sizeof(connection_t));
	if (_list == NULL) {
		LOG_E("malloc connection pool size");
		return -1;
	}

	//treat connection_t data field as the next pointer
	int i = 0;
	for (; i<_pool_size-1; i++) {
		(_list + i)->data = _list + i + 1;
	}
	(_list + i)->data = NULL;

	_free_list = _list;
	_free_size = _pool_size;

	return 0;
}

connection_t * ConnectionPool::get()
{
	if (_free_list == NULL) {
		LOG_W("no free connection_t");
		return NULL;
	}
	connection_t *conn = _free_list;
	_free_list = (connection_t *)conn->data;
	_free_size--;

	return conn;
}

void ConnectionPool::free(connection_t *conn)
{
	conn->data = _free_list;
	_free_list = conn;
	_free_size++;
}

