
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static int wait = 0;

typedef void (*sighandler_t)(int);

void handleINT(int signum){
  printf("\nOtrzymano SIGINT\n");
  exit(0);
}

void handleTSTP(int signum){
  //printf("%d\n", signum);
  printf("\n");
  if(signum == SIGTSTP){
    if(wait == 0){

      printf("\nOczekuje na:\nCTRL+Z - kontynuacja\nCTR+C - zakonczenie programu\n");
      wait = 1;

      sigset_t empty_mask;
      sigemptyset(&empty_mask);

      while(wait){
	sigsuspend(&empty_mask);
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

  while(1){
    system("date +%H:%M");//("date", "date", "+%H:%M", NULL);
    sleep(1);
  }

  return 0;
}
