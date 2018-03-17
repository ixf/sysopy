CC=gcc
CFLAGS=-g -Wall -std=c99

all: main

main: main.o generate.o sort_sys.o sort_lib.o copy_sys.o copy_lib.o timing.o
	$(CC) $(CFLAGS) $^ -o $@ 


%_sys.o: ../zad1a/%.c
	$(CC) -c $(CFLAGS) $^ -o $@ -D MAKE_WITH_SYS

%_lib.o: ../zad1a/%.c
	$(CC) -c $(CFLAGS) $^ -o $@ -D MAKE_WITH_LIB 

main.o: main.c
	$(CC) -c $(CFLAGS) $^ -o $@

timing.o: timing.c
	$(CC) -c $(CFLAGS) $^ -o $@

%.o: ../zad1a/%.c
	$(CC) -c $(CFLAGS) $^ -o $@

%: %.o
	$(CC) $^ -o $@

.PHONY: clean

clean:
	rm -rf *.o main
	rm -f wyniki.txt

