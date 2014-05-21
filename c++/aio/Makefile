

CC = g++
CPPFLAGS = -I.

DEPS = common.h
OBJS = aio.o aserver.o asocket.o xheadsocket.o connpool.o mempool.o reactor.o epollx.o watcher.o utils.o

all: server

server: $(OBJS)
	$(CC) -o $@ $^ $(CPPFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -g -c -o $@ $< $(CPPFLAGS)


clean:
	rm $(OBJS) server
