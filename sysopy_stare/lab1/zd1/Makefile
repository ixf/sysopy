THIS_DIR=$(shell pwd)

lib_static: block_array.c
	gcc -c block_array.c -g
	ar rcs libblock_array.a block_array.o

lib_shared: block_array.c
	gcc -fPIC -c block_array.c
	gcc -shared -Wl,-soname,libblock_array.so.1 \
	-o libblock_array.so.1.0.1 block_array.o -lc
	ln --force -s libblock_array.so.1.0.1 libblock_array.so.1
	ln --force -s libblock_array.so.1 libblock_array.so


test_static: lib_static
	gcc -c test.c -D TEST_STATIC -g
	gcc test.o block_array.o -o test -g

test_shared: lib_shared
	gcc test.c -lblock_array -L ./ -o test -D TEST_STATIC

test_dynamic: lib_shared
	gcc test.c -o test -D TEST_DYNAMIC -ldl

zad2_tests:
	LD_LIBRARY_PATH=$(THIS_DIR) ./test dynamic create_table 20000 128 add 15000 search_element 5000 remove_and_add 14000
	LD_LIBRARY_PATH=$(THIS_DIR) ./test static create_table add 15000 search_element 5000 remove_and_add 14000

zad3_tests:
	rm -f raport3.txt
	make test_static
	echo "Compiled with static libraries" >> raport3.txt
	make zad2_tests >> raport3.txt

	make test_shared
	echo "Compiled with shared libraries" >> raport3.txt
	LD_LIBRARY_PATH=$(THIS_DIR) make zad2_tests >> raport3.txt

	make test_dynamic
	echo "Compiled with dynamic libraries" >> raport3.txt
	LD_LIBRARY_PATH=$(THIS_DIR) make zad2_tests >> raport3.txt

zad3b_Os:
	gcc -c test.c -D TEST_STATIC -Os
	gcc test.o block_array.o -o test -Os
	./test dynamic create_table 20000 128 add 15000 search_element 5000 remove_and_add 14000

zad3b_O3:
	gcc -c test.c -D TEST_STATIC -O3
	gcc test.o block_array.o -o test -O3
	./test dynamic create_table 20000 128 add 15000 search_element 5000 remove_and_add 14000

zad3b_O0:
	gcc -c test.c -D TEST_STATIC -O0
	gcc test.o block_array.o -o test -O0
	./test dynamic create_table 20000 128 add 15000 search_element 5000 remove_and_add 14000

zad3b:
	rm -f raport3b.txt
	echo "Z flaga -Os" >> raport3b.txt
	make zad3b_Os >> raport3b.txt
	echo "Z flaga -O0" >> raport3b.txt
	make zad3b_O0 >> raport3b.txt
	echo "Z flaga -O3" >> raport3b.txt
	make zad3b_O3 >> raport3b.txt

