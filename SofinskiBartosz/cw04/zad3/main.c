#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

pid_t p;
int USR1count = 0;
int USR1received = 0;
int L, Type, SIG1, SIG2;

void handleUSR1(int signum, siginfo_t* sinfo, void* context){
  USR1count += 1;
  if(Type == 2 && p == 0)
    kill(getppid(), SIG1);
}

void handleINT(int signum){
  kill(p, 9);
  printf("Wyslano: \t%d\nOdebrano: \t%d\nFork odebral: \t%d\n", L, USR1count, USR1received);
  exit(0);
}

void handleUSR2(int signum, siginfo_t* sinfo, void* context){

  if(p != 0){
    USR1received = sinfo->si_value.sival_int;
    handleINT(0);
  } else {
    union sigval s;
    s.sival_int = USR1count;

    if( Type != 2 ){
      for( int i = 0; i < L; i++ ){
	kill(getppid(), SIG1);
      }
    }

    sigqueue(getppid(), SIG2, s); 
    exit(0);
  }
}

int main(int argc, char** argv){

  if( argc < 3 ){
    printf("not enough args\n");
    return -1;
  }

  L = strtol(argv[1], NULL, 10);
  Type = strtol(argv[2], NULL, 10);

  if( Type > 3 || Type < 1 || L < 0 ){
    printf("wrong args\n");
    return -1;
  }

  SIG1 = (Type == 3) ? SIGRTMIN : SIGUSR1;
  SIG2 = (Type == 3) ? SIGRTMIN+1 : SIGUSR2;

  sigset_t set12, set2, setint;
  sigfillset(&set12);
  sigfillset(&set2);
  sigfillset(&setint);
  sigdelset(&set12, SIG1);
  sigdelset(&set12, SIG2);
  sigdelset(&set2, SIG2);
  sigdelset(&set12, SIGINT);
  sigdelset(&set2, SIGINT);
  sigdelset(&setint, SIGINT);

  struct sigaction act; 
  sigfillset(&act.sa_mask); 
  sigdelset(&act.sa_mask, SIGINT);

  act.sa_flags = SA_SIGINFO;

  act.sa_sigaction = handleUSR2;
  sigaction(SIG2, &act, NULL);
  act.sa_sigaction = handleUSR1;
  sigdelset(&act.sa_mask, SIG1); 
  sigaction(SIG1, &act, NULL);

  p = fork();

  if( p != 0 ){ // parent

    if( Type == 2 )
      sigprocmask(SIG_SETMASK, &setint, NULL);

    signal(SIGINT, handleINT);
    
    for(int i = 0; i < L; i++){
      kill(p, SIG1);
      if( Type == 2 ) 
	sigsuspend(&set12); // czekaj na USR1
    }
    if( Type == 2 )
      sigprocmask(SIG_SETMASK, &set2, NULL); // czeka wylacznie na USR2
    kill(p,SIG2); 

    // odbieraj sygnaly w nieskonczonosc
    // konczy prace w handleUSR2
    while(1) pause();

  } else { // fork

    sigprocmask(SIG_SETMASK, &set12, NULL); // blokuj wszystko poza istnotnymi
    while(1) pause(); 
  }

  printf("Wyslano: \t%d\nOdebrano: \t%d\nFork odebral: \t%d\n", L, USR1count, USR1received);

  return 0;

}
