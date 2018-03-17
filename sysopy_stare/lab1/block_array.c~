#include "block_array.h"
#include <stdlib.h>

// funkcje dla obu wersji alokacji pamieci
#define SIZE 100
#define BLOCK_SIZE 32

char block_array[SIZE][BLOCK_SIZE];

int find_nearest(char** block_array, int elem, int elems, int block_size){
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
}

void delete_block_array(char** block_array, int elems){
	for(int i = 0; i < elems; i++){
		free(block_array[i]);
	}
	free(block_array);
}

/* Jako ze w programie korzystajacym z biblioteki musimy miec mozliwosc
tworzenia tablicy blokow o danym w momencie uruchamiania rozmiarze to nie
moze byc ona po prostu globalna. W tej wersji zdecydowalem sie na alokacje
wszystkich blokow w momencie tworzenia tablicy, co jest nieco bardziej
statycznym rozwiazaniem niz rozwiazanie powyzej.
Metoda alokacji pamieci jest przekazywana w parametrze, co pozwoli na
prostsza zmiane miedzy metodami alokacji w programie testujacym. */

char** create_block_array(int elems, int block_size, method m){
	char** t = (char**) calloc(elems, sizeof(char*));
	if(m == METHOD_STATIC)
		for(int i = 0; i < elems; i++){
			t[i] = calloc(block_size, sizeof(char));
		}
	return t;
}

char* create_block(char** block_array, int elem, int block_size, method m){
	if(m == METHOD_DYNAMIC)
		block_array[elem] = calloc(block_size, sizeof(char));
	return block_array[elem];
}

void delete_block(char** block_array, int elem, method m){
	if(m == METHOD_DYNAMIC)
		free(block_array[elem]);
}
