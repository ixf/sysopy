CC=gcc
CFLAGS=-g -Wall -std=c99 -D_GNU_SOURCE -lrt

all: server client

%: %.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -rf *.o server client

