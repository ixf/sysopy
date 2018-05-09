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

extern QueueDetails* q;
extern int * salon_queue;
extern sem_t* semid, *semid_waiting, *semid_sleep;
extern int shmid, shmid_q;

void pass(){}

int main(int argc, char** argv){

  if (argc < 3){
    printf("Not enough args\n");
    exit(1);
  }

  signal(SIGRTMIN, pass);

  int N = strtol(argv[1], NULL, 10);
  int S = strtol(argv[2], NULL, 10);

  int flags = 0;

  init_shared(flags);
  init_queue(q, flags);

  for( int i = 0; i < N; i++){
    if ( fork() == 0 ){

      for( int j = 0; j < S; j++){

	int val;
	sem_getvalue(semid, &val);
	printf("semid: %d\n", val);

	if( sem_trywait(semid) == 0 ){ // jezeli jest wolne
	  sem_post(semid_sleep); // obudz golibrode
	  q->seat = getpid(); // siadaj
	  printf("Siadam a kolejka pusta\n");
	  pause();
	  printf("Koniec\n");
	} else { // ustaw sie w kolejce
	  sem_wait(semid_waiting);
	  if ( q->taken != q->size){ // kiedy niepelna
	    int pos = (q->beg + q->taken) % q->size;
	    q->taken += 1;
	    printf("Ustawiam sie na %d pozycji\n", pos);
	    salon_queue[pos] = getpid();

	    sem_post(semid_waiting);

	    pause();
	    printf("Siadam\n");
	    pause();
	    printf("Koniec\n");
	  } else {
	    printf("kolejka pelna, opuszczam salon\n");
	    sem_post(semid_waiting);
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

