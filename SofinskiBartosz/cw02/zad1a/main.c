
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "sort.h"
#include "generate.h"
#include "copy.h"


int main(int argc, char** argv){

  if( argc < 5 ){
    printf("Not enough args\n");
    return 10;
  }

  int argi = 2;
  int task;
  char* source, *target;
  int records, record_length, method;

  if( strcmp( argv[1], "generate" ) == 0 ){
    task = 0;
  } else if ( strcmp( argv[1], "sort") == 0 ){
    task = 1;
  } else if ( strcmp( argv[1], "copy") == 0 ){
    task = 2;
  } else {
    printf("Unrecognized option: %s\n", argv[1]);
    return -1;
  }

  source = argv[argi];
  if( task == 2 ){
     target = argv[++argi];
  }

  if( sscanf(argv[++argi], "%d", &records) != 1 ||
      sscanf(argv[++argi], "%d", &record_length) != 1) {

	printf("Error at parsing argument %d\n", argi);
	return -1;
      }

  method = 0;
  if( task != 0 ){
    if( argc >= 6 ){
      if( strcmp(argv[++argi], "sys") == 0 ){
	method = 0;
      } else if( strcmp(argv[argi], "lib") == 0 ){
	method = 1;
      } else {
	printf("Unrecognized option: %s\n", argv[argi]);
	return -1;
      }
    } else {
      printf("Missing lib/sys argument\n");
      return -1;
    }
  }

  // 0 oznacza read write
  // 1 oznacza fread fwrite 

  if( task == 0 ){ // generate 

    return generate(source, records, record_length);

  } else if ( task == 1 ){ // sort

    if( method == 1 ){
      return sort_lib(source, records, record_length);
    } else {
      return sort_sys(source, records, record_length);
    }

  } else { // copy

    if( method == 1 ){
      return copy_lib(source, target, records, record_length);
    } else {
      return copy_sys(source, target, records, record_length);
    }

  }



  return 0;
}
