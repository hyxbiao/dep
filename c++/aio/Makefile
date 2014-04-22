
aio: aio.o epoll.o
	g++ -o aio aio.o epoll.o

aio.o: aio.cpp
	g++ -g -c aio.cpp

epoll.o: epoll.cpp epoll.h
	g++ -g -c epoll.cpp

clean:
	rm *.o aio
