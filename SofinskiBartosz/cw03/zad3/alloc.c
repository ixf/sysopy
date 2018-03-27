#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// przyjmuje ilosc pamieci w MB

size_t eaten_memory = 0;
void *memory = NULL;

int eat_kilobyte() {
	if (memory == NULL)
		memory = malloc(1024);
	else
		memory = realloc(memory, (eaten_memory * 1024) + 1024);
	if (memory == NULL)
	{
		return 1;
	}
	else
	{
		((char*)memory)[1024*eaten_memory] = 42;

		eaten_memory++;
		return 0;
	}
}

int main(int argc, char** argv){

	if(argc < 2){
		printf("not enough args\n");
		return -1;
	}

	int mbs = strtol(argv[1], NULL, 10);
	int i;
	for(i = 0; i < 1024*mbs; i++)
		eat_kilobyte();

	sleep(2);
	free(memory);

	return 0;
}
