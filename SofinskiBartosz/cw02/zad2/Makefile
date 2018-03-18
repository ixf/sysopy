CC=gcc
CFLAGS=-g -Wall -std=c99

all: main_dirent main_nftw

%: %.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@


.PHONY: clean

clean:
	rm -rf *.o main_dirent main_nftw

