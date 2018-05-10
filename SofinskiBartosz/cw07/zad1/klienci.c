#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "init_shared.h"

extern volatile Salon* q;
extern volatile int * salon_queue;
extern struct sembuf sleeping, wake, sit_down, cut, leave, done, enter_wr, leave_wr;
extern int sem, shmid, shmid_q;

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

	semop(sem, &enter_wr, 1);

	if( q->sleeping == 1 ){ // jezeli golibroda spi ( ma zero klientow )

	  q->sleeping = 0;
	  semop(sem, &wake, 1);
	  semop(sem, &leave_wr, 1);

	  printf("%d: Budze golibrode\n", getpid());

	  q->seat = getpid(); // siadaj
	  printf("%d: Siadam na fotelu (a kolejka byla pusta)\n", getpid());
	  semop(sem, &sit_down, 1); // po wpisaniu sie na fotel
	  semop(sem, &leave, 1); // czeka na koniec strzyzenia
	  printf("%d: Koniec golenia\n", getpid());

	} else { // ustaw sie w kolejce

	  if ( q->taken != q->size){ // kiedy niepelna
	    int pos = (q->beg + q->taken) % q->size;
	    q->taken += 1;
	    salon_queue[pos] = getpid();
	    printf("%d: Ustawiam sie na %d  miejscu w kolejce ( %d fizycznie )\n", getpid(), q->taken, pos);

	    semop(sem, &leave_wr, 1);

	    sigsuspend(&waiting_mask);

	    q->seat = getpid(); // siadaj
	    printf("%d: Siadam na fotelu\n", getpid());
	    semop(sem, &sit_down, 1); // po wpisaniu sie na fotel
	    semop(sem, &leave, 1); // czeka na koniec golenia
	    printf("%d: Koniec golenia\n", getpid());

	  } else {
	    semop(sem, &leave_wr, 1);
	    printf("%d: kolejka pelna, opuszczam salon\n", getpid());
	  }
	}
      }

      exit(0);

    }
  }

  printf("Klienci stworzeni\n");
  for( int i = 0; i < N; i++ ){
    wait(NULL);


  printf("Koniec strzyzen\n");
  return 0;

}

