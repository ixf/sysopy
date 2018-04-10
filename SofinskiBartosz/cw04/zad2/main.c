#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <sys/wait.h>

int received_signals = 0;
pid_t p;
pid_t* pids,* waiting; // procesy potomne
int N,K;
char tracking;

int wait_more = 0;

#define TRACK_PIDS 1 // pidy tworzonych procesow
#define TRACK_REQS 2 // otrzymane prosby
#define TRACK_SENT 4 // wysylane pozwolenia
#define TRACK_RT   8 // co zwraca proces i jaki sygnal
#define TRACK_EXIT 16 // ktory proces sie zakonczyl i co wzrocil

void handleRT(int signum, siginfo_t* sinfo, void* context){
  if( tracking & TRACK_RT )
    printf("Proces %d wyslal sygnal RT %d z wartoscia %d\n", sinfo->si_pid, signum, sinfo->si_value.sival_int);
}

void handleUSR1(int signum, siginfo_t* sinfo, void* context){

  if( p != 0 ){
    pid_t signal_sender = sinfo->si_pid;
    if( tracking & TRACK_REQS )
	printf("Otrzymano prosbe od %d\n", signal_sender);

    received_signals += 1;
    if ( received_signals < K){
	waiting[received_signals-1] = signal_sender;

    } else if (received_signals == K){

      waiting[received_signals-1] = signal_sender;
      for(int i = 0; i < K; i++){
	kill(waiting[i], SIGUSR1);

	if( tracking & TRACK_SENT )
	  printf("Wysylam pozwolenie do %d\n", waiting[i]);
      }
    } else {
	kill(signal_sender, SIGUSR1);
	if( tracking & TRACK_SENT )
	  printf("Wysylam pozwolenie do %d\n", signal_sender);
    }
  }
}

void handleINT(int signum){
  for(int i = 0; i < N; i++){
    kill(pids[i], 9);
  }
  free(pids);
  free(waiting);
  exit(0);
}

int main(int argc, char** argv){

  if( argc < 3){
    printf("not enough args\n");
    return -1;
  }

  N = strtol(argv[1], NULL, 10);
  K = strtol(argv[2], NULL, 10);

  if( N <= 0 || K <= 0 || K > N ){
    printf("wrong args\n");
    return -1;
  }

  for(int i = 3; i < argc; i++){
    if( strcmp(argv[i], "all") == 0){
      printf("Parent pid: %d\n", getpid());
      tracking = 31;
    } else if( strcmp(argv[i], "pids") == 0){
      tracking |= TRACK_PIDS;
    } else if (strcmp(argv[i], "reqs") == 0) {
      tracking |= TRACK_REQS;
    } else if (strcmp(argv[i], "sent") == 0) {
      tracking |= TRACK_SENT;
    } else if (strcmp(argv[i], "rt") == 0) {
      tracking |= TRACK_RT;
    } else if (strcmp(argv[i], "exit") == 0) {
      tracking |= TRACK_EXIT;
    }
  }
  
  signal(SIGINT, handleINT);
  signal(SIGALRM, handleINT);
  struct sigaction act; 

  sigfillset(&act.sa_mask); 

  act.sa_sigaction = handleUSR1; 
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGUSR1, &act, NULL);

  act.sa_sigaction = handleRT; 
  for(int i = SIGRTMIN; i <= SIGRTMAX; i++)
    sigaction(i, &act, NULL); 

  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, SIGUSR1);
  sigdelset(&mask, SIGINT);

  pids = malloc(N*sizeof(pid_t));
  waiting = malloc(N*sizeof(pid_t));

  for(int i = 0; i < N; i++){
    p = fork();

    if(p == 0){
      pid_t this = getpid();
      srand(time(NULL) + this);

      union sigval s;
      s.sival_int = rand() % 11;
      sleep(s.sival_int);

      kill(getppid(), SIGUSR1);
      sigsuspend(&mask);

      int sig = rand()%(SIGRTMAX-SIGRTMIN) + SIGRTMIN;
      sigqueue(getppid(), sig, s);
      exit(0); // albo exit(s);
    }

    if( tracking & TRACK_PIDS )
      printf("Utworzono proces %d\n", p);
    pids[i] = p;
  }

  alarm(12);
  int status;
  for(int i = 0; i < N; i++){
    p = waitpid(0, &status, 0);

    if( p == -1 ) i -= 1;
    else if ( tracking & TRACK_EXIT )
      if ( WIFEXITED(status) ){
	printf("Proces %d zakonczyl sie z %d\n", p, WEXITSTATUS(status));
      }
      
  }

  free(pids);
  free(waiting);
  return 0;
}
			 
