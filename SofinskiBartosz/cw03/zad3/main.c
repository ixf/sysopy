#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv){

  if(argc < 4){
    printf("Not enough arguments\n");
    return -1;
  }

  FILE* f = fopen(argv[1], "r");
  if( f == NULL ){
    printf("File does not exist\n");
    return -2;
  }

  struct rlimit cpu_limit, mem_limit;

  cpu_limit.rlim_max = strtol(argv[2], NULL, 10);
  mem_limit.rlim_max = 1024*1024*strtol(argv[3], NULL, 10); //todo errno
  cpu_limit.rlim_cur = cpu_limit.rlim_max;
  mem_limit.rlim_cur = mem_limit.rlim_max;

  char* line = NULL;
  size_t n = 0;
  
  pid_t t;
  int status;
  char* args[64];
  struct rusage usage_end; // usage_start;

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

      setrlimit(RLIMIT_CPU, &cpu_limit);
      setrlimit(RLIMIT_AS, &mem_limit);

      printf("$ ");

      int j;
      for(j = 0; args[j] != NULL; j++)
	printf("%s ", args[j]);
      printf("\n");

      if( execvp(args[0], args) == -1)
	exit(1);

    } else {

      /*
	getrusage(RUSAGE_CHILDREN, &usage_start);
	wait(&status);
	getrusage(RUSAGE_CHILDREN, &usage_end);
      */
      wait3(&status, 0, &usage_end);
      int code;
      if( WIFEXITED(status) && (code = WEXITSTATUS(status)) != 0 ){
	printf("Error: %s has returned %d\n", args[0], code);
	return -1;
      }
      if( WIFSIGNALED(status) ){
	printf("Warning: %s was terminated\n", args[0]);
	//printf("%d\n", WTERMSIG(status)); // zawsze 9 wychodzilo
      }

      printf("Czas:\tsystem:\t%f\n\tuser:\t%lf\n",
	     usage_end.ru_stime.tv_sec + usage_end.ru_stime.tv_usec/1000000.0
	     ,// - usage_start.ru_stime.tv_sec - usage_start.ru_stime.tv_usec/1000000.0,
	     usage_end.ru_utime.tv_sec + usage_end.ru_utime.tv_usec/1000000.0
	     //- usage_start.ru_utime.tv_sec - usage_start.ru_utime.tv_usec/1000000.0);
	     );
    }
  }

  free(line);

  return 0;
}
