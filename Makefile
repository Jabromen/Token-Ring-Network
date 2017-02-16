CFLAGS = -g -Wall
CC = gcc

all: bbpeer bbserver

bbpeer:	peer.c udpsockets.c queue.c read_token.c
	$(CC) $(CFLAGS) -pthread -o bbpeer peer.c udpsockets.c queue.c read_token.c

bbserver: server.c udpsockets.c
	$(CC) $(CFLAGS) -o bbserver server.c udpsockets.c

.PHONY: clean
clean:
	rm bbpeer bbserver