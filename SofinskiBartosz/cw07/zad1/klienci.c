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

QueueDetails* q;
int * salon_queue;
extern int semid, semid_waiting, shmid, shmid_q;

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

  struct sembuf op;
  op.sem_num = 0;
  op.sem_op = 1;
  op.sem_flg = 0;

  QueueDetails* q = init_shared(flags);
  init_queue(q, flags);

  for( int i = 0; i < N; i++){
    if ( fork() == 0 ){

      for( int j = 0; j < S; j++){

	op.sem_op = -1;
	op.sem_flg = IPC_NOWAIT;
	if( semop(semid, &op, 1) == 0 ){ // jezeli jest wolne
	  q->seat = getpid(); // siadaj
	  printf("Siadam\n");
	  pause();
	  printf("Koniec\n");
	} else { // ustaw sie w kolejce
	  op.sem_op = -1;
	  op.sem_flg = 0;
	  semop(semid_waiting, &op, 1);
	  if ( q->beg != ( q->end + 1) % (q->size)){ // kiedy niepelna
	    int pos = q->end;
	    q->end = ( pos + 1) % q->size;
	    printf("Ustawiam sie na %d pozycji\n", pos);
	    salon_queue[pos] = getpid();

	    op.sem_op = 1;
	    semop(semid_waiting, &op, 1); // zwalniam blokade

	    pause();
	    printf("Siadam\n");
	    pause();
	    printf("Koniec\n");
	  } else {
	    printf("kolejka pelna, opuszczam salon\n");
	    op.sem_op = 1;
	    semop(semid_waiting, &op, 1); // zwalniam blokade
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

