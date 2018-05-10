#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "init_shared.h"

extern Salon* q;
extern int * salon_queue;
extern sem_t* sem[8];

void pass(){}

int main(int argc, char** argv){

  if (argc < 3){
    printf("Not enough args\n");
    exit(1);
  }

  signal(SIGRTMIN, pass);
  sigset_t regular_mask, waiting_mask;
  sigemptyset(&regular_mask);
  sigemptyset(&waiting_mask);
  sigaddset(&regular_mask, SIGRTMIN);
  sigprocmask(SIG_SETMASK, &regular_mask, NULL);

  int N = strtol(argv[1], NULL, 10);
  int S = strtol(argv[2], NULL, 10);

  init_shared(0);
  init_queue(0);

  for( int i = 0; i < N; i++){
    if ( fork() == 0 ){

      for( int j = 0; j < S; j++){

	sem_wait(sem[enter_wr]);

	if( q->sleeping == 1 ){ // jezeli jest wolne

	  q->sleeping = 0;
	  sem_post(sem[sleeping]);
	  sem_post(sem[enter_wr]);

	  q->seat = getpid();
	  printf("%d: Siadam na fotelu (a kolejka byla pusta)\n", getpid());
	  sem_post(sem[cut]); 
	  sem_wait(sem[leave]);
	  printf("%d: Koniec golenia\n", getpid());

	} else {

	  // ustaw sie w kolejce
	  if ( q->taken != q->size){ // kiedy niepelna
	    int pos = (q->beg + q->taken) % q->size;
	    q->taken += 1;
	    printf("%d: Ustawiam sie na %d  miejscu w kolejce ( %d fizycznie )\n", getpid(), q->taken, pos);
	    salon_queue[pos] = getpid();

	    sem_post(sem[enter_wr]);

	    sigsuspend(&waiting_mask);
	    q->seat = getpid();
	    printf("%d: Siadam na fotelu\n", getpid());
	    sem_post(sem[cut]); 
	    sem_wait(sem[leave]);
	    printf("%d: Koniec golenia\n", getpid());

	  } else {
	    sem_post(sem[enter_wr]);
	    printf("%d: kolejka pelna, opuszczam salon\n", getpid());
	  }
	}
      }
      exit(0);

    }
  }

  printf("Klienci stworzeni\n");
  for( int i = 0; i < N; i++ )
    wait(NULL);

  printf("Koniec strzyzen\n");

}

