
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static int wait = 0;

typedef void (*sighandler_t)(int);

sighandler_t handleInt(int signum){
  printf("\nOtrzymano SIGINT\n");
  exit(0);
}

sighandler_t handlePause(int signum){
  printf("%d\n", signum);

  if(signum == SIGTSTP){
    if(wait == 0){

      printf("\nOczekuje na:\nCTRL+Z - kontynuacja\nCTR+C - zakonczenie programu\n");
      wait = 1;

      sigset_t empty_mask;
      sigemptyset(&empty_mask);

      while(wait){
	sigsuspend(&empty_mask);
      }
    } else
      wait = 0;
  } else if (signum == SIGINT && wait == 1){
    exit(0);
  }
  return 0;
}


int main(){
  signal(SIGINT, *handleInt);

  struct sigaction act; 
  act.sa_handler = handlePause; 
  sigemptyset(&act.sa_mask); 
  act.sa_flags = 0; 
  sigaction(SIGTSTP, &act, NULL); 

  while(1){
    pid_t p = fork();
    if( p == 0 ){
      execlp("date", "date", "+%H:%M", NULL);
      exit(2);
    }
    sleep(1);
  }


  return 0;
}
