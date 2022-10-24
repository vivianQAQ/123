CC = gcc
CFLAGS = -g -O0
CFLAGS += -Wall -Werror

all: client server

client: client.c
	$(CC) -o $@ $^ $(CFLAGS)

server: server.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf client server

.PHONY: clean client server
