

lib_static: block_array.c
	gcc -c block_array.c
	ar rcs libblock_array.a block_array.o

lib_shared: block_array.c
	gcc -fPIC -c block_array.c
	gcc -shared -Wl,-soname,libblock_array.so.1 \
	-o libblock_array.so.1.0.1 block_array.o -lc
	ln --force -s libblock_array.so.1.0.1 libblock_array.so.1
	ln --force -s libblock_array.so.1 libblock_array.so




test_static: lib_static
	gcc -c test.c -D TEST_STATIC
	gcc test.o block_array.o -o test

test_shared: lib_shared
	gcc test.c -lblock_array -L ./ -o test -D TEST_DYNAMIC -ldl

test_dynamic: lib_shared
	gcc test.c -o test -D TEST_DYNAMIC -ldl -g
	

perform_testing: test_static
	
