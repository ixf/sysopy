CC=gcc
CFLAGS=-g -Wall -std=c99 -D_GNU_SOURCE -lpthread -lm -O3

all: main

main: main.o timing.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -rf *.o main threads
