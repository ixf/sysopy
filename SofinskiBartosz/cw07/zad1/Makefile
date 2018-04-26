CC=gcc
CFLAGS=-g -Wall -std=c99 -D_GNU_SOURCE

all: golibroda klienci

golibroda: golibroda.o init_shared.o
	$(CC) $(CFLAGS) $^ -o $@

klienci: klienci.o init_shared.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -rf *.o golibroda klienci
