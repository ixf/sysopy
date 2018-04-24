CC=gcc
CFLAGS=-g -Wall -std=c99

all: main

%: %.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -rf *.o main
	rm -f out

