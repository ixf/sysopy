CC=gcc
CFLAGS=-g -Wall

all: master slave

%: %.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -rf *.o main

dbg:
	gdb --args ./main inputs

