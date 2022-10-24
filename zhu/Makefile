CC = gcc
CFLAGS = -g -O0 -Wall -Werror

all: demo cli_demo srv_demo

demo: demo.c pel.c
	$(CC) -o $@ $^ $(CFLAGS)

cli_demo: cli_demo.c pel.c
	$(CC) -o $@ $^ $(CFLAGS)

srv_demo: srv_demo.c pel.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf demo cli_demo srv_demo

.PHONY: demo clean
