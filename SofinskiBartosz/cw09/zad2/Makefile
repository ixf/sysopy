CC=gcc
CFLAGS=-g -Wall -std=c99 -D_GNU_SOURCE -lpthread -lm

all: main

pantadeusz:
	wget https://wolnelektury.pl/media/book/txt/pan-tadeusz.txt

test: main
	./main config

main: main.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean pantadeusz

clean:
	rm -rf *.o main
