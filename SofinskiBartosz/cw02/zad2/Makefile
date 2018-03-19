CC=gcc
CFLAGS=-g -Wall -std=gnu99 -D_BSD_SOURCE -D_DEFAULT_SOURCE

all: main_dirent main_nftw

%_dirent: %_dirent.o
	$(CC) $^ -o $@

%_nftw: %_nftw.o
	$(CC) $^ -o $@

%_dirent.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@ -D MAKE_WITH_DIRENT

%_nftw.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@ -D MAKE_WITH_NFTW

.PHONY: clean

clean:
	rm -rf *.o main_dirent main_nftw

