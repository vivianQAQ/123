CC = gcc
CFLAGS = -Wall -Werror -g -O0

all: sock_cli sock_srv

sock_cli: sock_cli.c channels.c
	$(CC) -o $@ $^ $(CFLAGS)

sock_srv: sock_srv.c channels.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf sock_cli sock_srv

.PHONY: sock_cli sock_srv clean
