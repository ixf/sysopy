
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define FAIL(text, ...) { fprintf(stderr, text, ##__VA_ARGS__); exit(1); }

int main(int argc, char** argv){

  if(argc < 3) FAIL("not enough arguments!\n");
  int f = open(argv[1], O_WRONLY);
  int N = strtol(argv[2], NULL, 10);
  if( !f ) FAIL("failed to open file!\n");

  const char *t = "eyy slave here\n\0";
  srand(time(NULL));

  while(N > 0){
    sleep( rand()%3+2 );
    write(f, t, 20 );
    N-=1;
  }

  close(f);
}
