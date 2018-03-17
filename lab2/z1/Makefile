CC=gcc
CFLAGS=-g -Wall

all: main

main: main.o generate.o sort_sys.o sort_lib.o
	$(CC) $(CFLAGS) $^ -o $@ 

sort_sys.o: sort.c
	$(CC) -c $(CFLAGS) $^ -o $@ -D SORT_WITH_SYS

sort_lib.o: sort.c
	$(CC) -c $(CFLAGS) $^ -o $@ -D SORT_WITH_LIB 

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

%: %.o
	$(CC) $^ -o $@

.PHONY: clean
	
clean:
	rm -rf *.o main
	rm kappa.txt

