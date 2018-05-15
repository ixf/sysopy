CC=gcc
CFLAGS=-g -Wall -std=c99 -D_GNU_SOURCE -lpthread -lm

all: threads

%: %.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -rf *.o main threads
