#include <unistd.h>
#include <stdio.h>

int main(){
	printf("%ld\n", sysconf(_SC_CLK_TCK));
}
