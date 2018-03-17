#include "block_array.h"
#include <stdlib.h>
#include <string.h>

#ifndef SIZE
#define SIZE 20000
#endif

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 128
#endif

char block_array[SIZE][BLOCK_SIZE];

char** create_block_array_dynamic(int elems, int block_size){
  char** t;
    t = (char**) calloc(elems, sizeof(char*));
  return t;
}

void create_block_dynamic(char** block_array, int elem, int block_size){
  block_array[elem] = calloc(block_size, sizeof(char));
  for(int i = 0; i < block_size; i++){
    block_array[elem][i] = 'A' + (rand() % 26) + (rand()%2)*24;
  }
}

void create_block_static(int elem){
  for(int i = 0; i < BLOCK_SIZE; i++){
    block_array[elem][i] = 'A' + (rand() % 26) + (rand()%2)*24;
  }
}

void delete_block_dynamic(char** block_array, int elem, int block_size){
  free(block_array[elem]);
}

void delete_block_static(int elem){
  memset(block_array[elem], 0, sizeof(char)*BLOCK_SIZE);
}

void delete_block_array_dynamic(char** block_array, int elems){
  for(int i = 0; i < elems; i++){
    free(block_array[i]);
  }
  free(block_array);
}

int find_nearest_dynamic(char** block_array, int elem, int elems, int block_size){
  int given_sum = 0;
  for(int i = 0; i < block_size; i++){
    given_sum += block_array[elem][i];
  }
	
  int nearest = 0;
  int nearest_diff = 0;

  for(int i = 0; i < elems; i++){
    if ( i != elem){
      int sum = 0;
      for(int j = 0; j < block_size; j++){
	sum += block_array[i][j];
      }

      if( abs( given_sum - sum ) < nearest_diff ){
				
	nearest = i;
	nearest_diff = abs(given_sum - sum);

      }
    }
  }

  return nearest;
}

int find_nearest_static(int elem){
  int given_sum = 0;
  for(int i = 0; i < BLOCK_SIZE; i++){
    given_sum += block_array[elem][i];
  }
	
  int nearest = 0;
  int nearest_diff = 0;

  for(int i = 0; i < SIZE; i++){
    if ( i != elem){
      int sum = 0;
      for(int j = 0; j < BLOCK_SIZE; j++){
	sum += block_array[i][j];
      }

      if( abs( given_sum - sum ) < nearest_diff ){
				
	nearest = i;
	nearest_diff = abs(given_sum - sum);

      }
    }
  }

  return nearest;
}
