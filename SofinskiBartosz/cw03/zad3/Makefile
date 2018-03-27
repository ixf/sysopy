CC=gcc
CFLAGS=-g -Wall

all: main ack alloc

%: %.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -rf *.o main ack alloc

