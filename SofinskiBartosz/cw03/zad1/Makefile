CC=gcc
CFLAGS=-g -Wall -std=gnu99 -D_BSD_SOURCE -D_DEFAULT_SOURCE

all: main

%: %.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -rf *.o main

