#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t ack(uint64_t m, uint64_t n){
	if( m == 0 ) return n+1;
	if( n == 0 ) return ack(m-1,1);
	return ack(m-1,ack(m,n-1));
}

int main(int argc, char** argv){
	if(argc < 3){
		printf("Not enough arguments\n");
		return -1;
	}

	int m = strtol(argv[1], NULL, 10);
	int n = strtol(argv[2], NULL, 10);

	printf("%ld\n", ack(m,n));
	return 0;
}
