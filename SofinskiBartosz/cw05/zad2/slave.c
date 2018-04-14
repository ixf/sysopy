
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

  char buf[64];
  sprintf(buf, "%d", getpid());
  char* date_buf = index(buf, 0);
  *date_buf = ' ';
  date_buf += 1;
  
  srand(time(NULL));

  while(N > 0){
    sleep( rand()%3+2 );
    FILE* fork = popen("date +%H:%M:%S", "r");
    fread(date_buf, 64, 1, fork);
    *index(buf,'\n') = 0;
    printf("%s\n", buf);
    write(f, buf, 20 );
    N-=1;
    pclose(fork);
  }

  close(f);
}
