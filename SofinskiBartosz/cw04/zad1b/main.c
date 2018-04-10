
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static int wait = 0;

typedef void (*sighandler_t)(int);
pid_t p;

void handleINT(int signum){
  kill(p, 9);
  printf("\nOtrzymano SIGINT\n");
  exit(0);
}

void handleTSTP(int signum){
  //printf("%d\n", signum);
  printf("\n");
  if(signum == SIGTSTP){
    if(wait == 0){

      kill(p, 9);
      printf("\nOczekuje na:\nCTRL+Z - kontynuacja\nCTR+C - zakonczenie programu\n");
      wait = 1;

      sigset_t empty_mask;
      sigemptyset(&empty_mask);

      while(wait){
	sigsuspend(&empty_mask);
      }
      p = fork();
      if( p == 0 ){

	execlp("date.sh", "date.sh", NULL);
	exit(2);

      }
    } else {
      wait = 0;
    }
  }
}

int main(){
  signal(SIGINT, *handleINT);

  struct sigaction act; 
  act.sa_handler = handleTSTP; 
  sigemptyset(&act.sa_mask); 
  act.sa_flags = 0; 
  sigaction(SIGTSTP, &act, NULL); 

  p = fork();
  if( p == 0 ){
    execlp("date.sh", "date.sh", NULL);
    exit(2);
  }

  while(1){
    sleep(10);
  }

  return 0;
}
