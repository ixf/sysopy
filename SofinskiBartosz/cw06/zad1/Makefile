CC=gcc
CFLAGS=-g -Wall -std=c99 -D_XOPEN_SOURCE

all: main

%: %.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -rf *.o main

dbg:
	gdb --args ./main inputs

