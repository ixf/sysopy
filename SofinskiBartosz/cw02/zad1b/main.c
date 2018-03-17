
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "../zad1a/sort.h"
#include "../zad1a/generate.h"
#include "../zad1a/copy.h"
#include "timing.h"

static FILE* wyniki;

static int buffer_sizes[4] = { 4, 512, 4096, 8192 };
static int records[2] = { 2000, 3500 };

int main(int argc, char** argv){
  wyniki = fopen("wyniki.txt", "w");

  for(int j = 0; j < 2; j++)
    for(int i = 0; i < 4; i++){
	printf("Testy dla: rozmiar %d, ilosc rekordow %d\n", buffer_sizes[i], records[j]);
	generate("dane1", records[j], buffer_sizes[i]);

	printf("copy_sys\n");
	start_time();
	copy_sys("dane1", "dane2", records[j], buffer_sizes[i]);
	end_time();

	printf("copy_lib\n");
	start_time();
	copy_lib("dane1", "dane3", records[j], buffer_sizes[i]);
	end_time();

	printf("sort_sys\n");
	start_time();
        sort_sys("dane2", records[j], buffer_sizes[i]);
	end_time();

	printf("sort_lib\n");
	start_time();
	sort_lib("dane3", records[j], buffer_sizes[i]);
	end_time();
    }

  fclose(wyniki);
}
