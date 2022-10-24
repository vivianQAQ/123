CC = gcc
CFLAGS = -g -O0
CFLAGS += -Wall -Werror -lcrypto

all: client server

client: client.c common.c
	$(CC) -o $@ $^ $(CFLAGS)

server: server.c common.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf client server

.PHONY: clean client server
