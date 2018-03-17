#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define REPS 40

typedef enum { METHOD_STATIC=0, METHOD_DYNAMIC=1 } method;

#ifdef TEST_STATIC
#include "block_array.h"
#else 
#include <dlfcn.h>
#endif

static struct timeval start_tv, end_tv;
static struct rusage start_usage, end_usage;

static FILE* file;

void print_time(FILE* stream){
  fprintf(stream, "Real time:\t%011ld usec\n", (end_tv.tv_sec - start_tv.tv_sec) * 1000000 +
	  end_tv.tv_usec - start_tv.tv_usec);
  fprintf(stream, "User time:\t%011ld usec\n", (end_usage.ru_utime.tv_sec - start_usage.ru_utime.tv_sec)*1000000 +
	  end_usage.ru_utime.tv_usec - start_usage.ru_utime.tv_usec);
  fprintf(stream, "System time:\t%011ld usec\n", (end_usage.ru_stime.tv_sec - start_usage.ru_stime.tv_sec)*1000000 +
	  end_usage.ru_stime.tv_usec - start_usage.ru_stime.tv_usec);
}

void time_start(){
  gettimeofday(&start_tv, NULL);
  getrusage(RUSAGE_SELF, &start_usage);
}

void time_end(){
  gettimeofday(&end_tv, NULL);
  getrusage(RUSAGE_SELF, &end_usage);

  print_time(stdout);
  print_time(file);
}

int main(int argc, char** argv){
  if( argc < 4 ){
    printf("Not enough arguments.\nUsage: test static/dynamic tasks");
    return 1;
  }

  file = fopen("outputs","w");

  srand(time(NULL));

#ifdef TEST_DYNAMIC
  void *handle = dlopen("./libblock_array.so", RTLD_LAZY);
  if(!handle){
    return 10;
  }


  char** (*create_block_array_dynamic)(int, int);
  void (*create_block_dynamic)(char**,int,int);
  void (*create_block_static)(int);
  void (*delete_block_dynamic)(char**,int,int);
  void (*delete_block_static)(int);
  void (*delete_block_array_dynamic)(char**,int);
  int (*find_nearest_dynamic)(char**,int,int,int);
  int (*find_nearest_static)(int);

  create_block_array_dynamic = dlsym(handle,"create_block_array_dynamic");
  create_block_dynamic = dlsym(handle,"create_block_dynamic");
  create_block_static = dlsym(handle,"create_block_static");
  delete_block_dynamic = dlsym(handle,"delete_block_dynamic");
  delete_block_static = dlsym(handle,"delete_block_static");
  delete_block_array_dynamic = dlsym(handle,"delete_block_array_dynamic");
  find_nearest_dynamic = dlsym(handle,"find_nearest_dynamic");
  find_nearest_static = dlsym(handle,"find_nearest_static");
#endif
	
  method m;
  if( strcmp( argv[1], "static") == 0){
    m = METHOD_STATIC;
    fprintf(stdout, "Static allocation:\n");
    fprintf(file, "Static allocation:\n");
  } else if ( strcmp( argv[1], "dynamic") == 0){
    m = METHOD_DYNAMIC;
    fprintf(stdout, "Dynamic allocation:\n");
    fprintf(file, "Dynamic allocation:\n");
  } else {
    return 11;
  }

  char** bar;
  int added = 0;
  int rozmiar, rozmiar_bloku; 

  for(int i = 2; i < argc; i++){
    if( strcmp( argv[i], "create_table") == 0){
      fprintf(stdout, "create_table:\n");
      fprintf(file, "create_table:\n");

      if(m == METHOD_DYNAMIC){
	rozmiar = strtol( argv[i+1], NULL, 10);
	rozmiar_bloku = strtol( argv[i+2], NULL, 10);
	i += 2;
      }

      time_start();
      if(m == METHOD_DYNAMIC){
	bar = create_block_array_dynamic(rozmiar, rozmiar_bloku);
      }
      time_end();

      fprintf(stdout, "\n");
      fprintf(file, "\n");

    } else if( strcmp( argv[i], "search_element") == 0){
      fprintf(stdout, "search_element:\n");
      fprintf(file, "search_element:\n");

      int index = strtol( argv[i+1], NULL, 10);
      time_start();
      if(m == METHOD_DYNAMIC){
	for(int i = 0; i < REPS; i++)
	  find_nearest_dynamic(bar, index, MIN(added,rozmiar), rozmiar_bloku);
      } else {
	for(int i = 0; i < REPS; i++)
	  find_nearest_static(index);
      }
      time_end();
      i += 1;

      fprintf(stdout, "\n");
      fprintf(file, "\n");

    } else if( strcmp( argv[i], "remove") == 0){
      fprintf(stdout, "remove:\n");
      fprintf(file, "remove:\n");
      int number = strtol( argv[i+1], NULL, 10);

      added -= number;

      time_start();
      for(int i = 0; i < number; i++)
	if(m == METHOD_DYNAMIC){
	  delete_block_dynamic(bar, i, rozmiar_bloku);
	} else {
	  delete_block_static(i);
	}

      time_end();
      i += 1;

      fprintf(stdout, "\n");
      fprintf(file, "\n");
    } else if( strcmp( argv[i], "add") == 0){
      fprintf(stdout, "add:\n");
      fprintf(file, "add:\n");
      int number = strtol( argv[i+1], NULL, 10);

      added += number;

      time_start();
      for(int j = 0; j < number; j++)
	if(m == METHOD_DYNAMIC){
	  create_block_dynamic(bar, j, rozmiar_bloku);
	} else {
	  create_block_static(j);
	}
      time_end();

      i += 1;

      fprintf(stdout, "\n");
      fprintf(file, "\n");
    } else if( strcmp( argv[i], "remove_and_add") == 0){
      fprintf(stdout, "remove_and_add:\n");
      fprintf(file, "remove_and_add:\n");
      int number = strtol( argv[i+1], NULL, 10);

      time_start();
      for(int i = 0; i < REPS; i++)
	if(m == METHOD_DYNAMIC){
	  for(int j = 0; j < number; j++)
	    delete_block_dynamic(bar, rozmiar-j, rozmiar_bloku);
	  for(int j = 0; j < number; j++)
	    create_block_dynamic(bar, rozmiar-j, rozmiar_bloku);
	} else {
	  for(int j = 0; j < number; j++)
	    delete_block_static(j);
	  for(int j = 0; j < number; j++)
	    create_block_static(j);
	}

      time_end();
      i += 1;
      fprintf(stdout, "\n");
      fprintf(file, "\n");
    }
  }

  if(m == METHOD_DYNAMIC)
    delete_block_array_dynamic(bar, rozmiar);
#ifdef TEST_DYNAMIC
  dlclose(handle);
#endif
  fclose(file);
  return 0;
}
