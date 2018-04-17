
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define FAIL(text, ...) { fprintf(stderr, text, ##__VA_ARGS__); exit(1); }

int main(int argc, char** argv){

  if(argc < 2) FAIL("not enough arguments!\n");
  mkfifo(argv[1], 0666);
  char buf[512];
  int f = open(argv[1], O_RDWR);
  if( !f ) FAIL("file does not exist!\n");

  while(1){
    if (read(f, buf, 512) > 0)
	printf("%s\n", buf);
  }

  close(f);
  unlink(argv[1]);
}
