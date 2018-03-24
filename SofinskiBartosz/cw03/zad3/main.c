#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char** argv){

  if(argc < 2){
    printf("Not enough arguments\n");
    return -1;
  }

  FILE* f = fopen(argv[1], "r");
  if( f == NULL ){
    printf("File does not exist\n");
    return -2;
  }

  char* line = NULL;
  size_t n = 0;

  
  pid_t t;
  int status;
  char* args[64];

  while( getline(&line, &n, f) != -1 ){

    args[0] = strtok(line, " ");

    int i = 0;
    do {
      i += 1;
      args[i] = strtok(NULL, " ");
    } while( i < 64 && args[i] != NULL );
    args[i-1][strlen(args[i-1]) -1] = 0;
    
    t = fork();
    if( t == 0 ){

      if( execvp(args[0], args) == -1)
	exit(1);

    } else {

      wait(&status);
      int code;
      if( WIFEXITED(status) && (code = WEXITSTATUS(status)) != 0 ){
	printf("Error: %s has returned %d\n", args[0], code);
	return -1;
      }

    }
  }

  free(line);

  return 0;
}
