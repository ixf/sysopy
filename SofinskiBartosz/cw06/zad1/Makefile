CC=gcc
CFLAGS=-g -Wall -std=c99 -D_GNU_SOURCE

all: server client

%: %.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -rf *.o client server
