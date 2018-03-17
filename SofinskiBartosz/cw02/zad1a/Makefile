CC=gcc
CFLAGS=-g -Wall

all: main

main: main.o generate.o sort_sys.o sort_lib.o copy_sys.o copy_lib.o
	$(CC) $(CFLAGS) $^ -o $@ 

%_sys.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@ -D MAKE_WITH_SYS

%_lib.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@ -D MAKE_WITH_LIB 

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

%: %.o
	$(CC) $^ -o $@

.PHONY: clean
	
clean:
	rm -rf *.o main
	rm -f kappa.txt

