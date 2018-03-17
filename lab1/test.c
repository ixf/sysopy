#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <sys/resource.h>

#ifdef TEST_STATIC
#include "block_array.h"
#else 
#include <dlfcn.h>
#endif

void fill_block(char** block_array, int elem, int block_size){
	for(int i = 0; i < block_size; i++){
		block_array[elem][i] = 'A' + (rand() % 26) + (rand()%2 )*24;
	}
}

void print_times(struct timeval start_tv, struct timeval end_tv,
		struct rusage start_usage, struct rusage end_usage, FILE* stream){
	fprintf(stream, "Real time:\t%011ld usec\n", (end_tv.tv_sec - start_tv.tv_sec) * 1000000 +
			end_tv.tv_usec - start_tv.tv_usec);
	fprintf(stream, "User time:\t%011ld usec\n", (end_usage.ru_utime.tv_sec - start_usage.ru_utime.tv_sec)*1000000 +
			end_usage.ru_utime.tv_usec - start_usage.ru_utime.tv_usec);
	fprintf(stream, "System time:\t%011ld usec\n", (end_usage.ru_stime.tv_sec - start_usage.ru_stime.tv_sec)*1000000 +
			end_usage.ru_stime.tv_usec - start_usage.ru_stime.tv_usec);
}


int main(int argc, char** argv){
	if( argc < 4 ){
		printf("Not enough arguments.\n");
		return 1;
	}

	srand(time(NULL));
#ifdef TEST_DYNAMIC
	void *handle = dlopen("./libblock_array.so", RTLD_LAZY);
	if(!handle){
		return 10;
	}
	
	char** (*create_block_array)(int,int,int);
	char* (*create_block)(char**,int,int,int);
	void (*delete_block)(char**,int,int);
	void (*delete_block_array)(char**,int);
	int (*find_nearest)(char**,int,int,int);

	create_block_array = dlsym(handle,"create_block_array");
	create_block = dlsym(handle,"create_block");
	delete_block = dlsym(handle,"delete_block");
	delete_block_array = dlsym(handle,"delete_block_array");
	find_nearest = dlsym(handle,"find_nearest");
	
#endif
	
	char** bar;

	int m = strtol(argv[1], NULL, 10);

	for(int i = 2; i < argc; i++){
		if( strcmp( argv[i], "create_table") == 0){
			int rozmiar = strtol( argv[i+1], NULL, 10);
			int rozmiar_bloku = strtol( argv[i+2], NULL, 10);
			i += 2;

			bar = create_block_array(rozmiar, rozmiar_bloku, m);

		} else if( strcmp( argv[i], "search_element") == 0){

		} else if( strcmp( argv[i], "remove") == 0){

		} else if( strcmp( argv[i], "add") == 0){

		} else if( strcmp( argv[i], "remove_and_add") == 0){

		}
	}

	struct timeval start_tv, end_tv;
	struct rusage start_usage, end_usage;
	gettimeofday(&start_tv, NULL);
	getrusage(RUSAGE_SELF, &start_usage);

	//bar = create_block_array(elems, block_size, m);
	//delete_block_array(bar, elems);

	gettimeofday(&end_tv, NULL);
	getrusage(RUSAGE_SELF, &end_usage);

	print_times(start_tv, end_tv, start_usage, end_usage, stdout);
#ifdef TEST_DYNAMIC
	dlclose(handle);
#endif
	return 0;
}


