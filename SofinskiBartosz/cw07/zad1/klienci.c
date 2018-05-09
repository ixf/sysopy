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
extern int semid, semid_waiting, semid_seat, shmid, shmid_q;

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
	op.sem_flg = 0;
	semop(semid_waiting, &op, 1);

	op.sem_op = 0;
	op.sem_flg = IPC_NOWAIT;

	if( semop(semid, &op, 1) == 0 ){ // jezeli golibroda ma zero klientow ( spi )

	  op.sem_op = 2; // 1 na obudzenie golibrody i 1 na klienta do obsluzenia
	  semop(semid, &op, 1);
	  op.sem_op = 1;
	  semop(semid_waiting, &op, 1);

	  printf("%d: Budze golibrode\n", getpid());

	  q->seat = getpid(); // siadaj
	  op.sem_op = 1;
	  printf("%d: Siadam na fotelu (a kolejka byla pusta)\n", getpid());
	  semop(semid_seat, &op, 1); // po wpisaniu sie na fotel
	  op.sem_op = -2;
	  op.sem_flg = 0;
	  semop(semid_seat, &op, 1); // czeka na koniec strzyzenia
	  printf("%d: Koniec golenia\n", getpid());

	} else { // ustaw sie w kolejce

	  if ( q->taken != q->size){ // kiedy niepelna
	    int pos = (q->beg + q->taken) % q->size;
	    q->taken += 1;
	    salon_queue[pos] = getpid();
	    printf("%d: Ustawiam sie na %d  miejscu w kolejce ( %d fizycznie )\n", getpid(), q->taken, pos);

	    op.sem_op = 1;
	    semop(semid_waiting, &op, 1); // zwalniam kolejke 

	    sigsuspend(&waiting_mask);

	    q->seat = getpid(); // siadaj
	    op.sem_op = 1;
	    printf("%d: Siadam na fotelu\n", getpid()); // wlasciwie to jest sadzany przez golibrode
	    semop(semid_seat, &op, 1); // po wpisaniu sie na fotel
	    op.sem_op = -2;
	    semop(semid_seat, &op, 1); // czeka na koniec golenia
	    printf("%d: Koniec golenia\n", getpid());

	  } else {
	    printf("%d: kolejka pelna, opuszczam salon\n", getpid());
	    op.sem_op = 1;
	    semop(semid_waiting, &op, 1); // zwalniam blokade
	  }
	  op.sem_op = -1;
	  op.sem_flg = IPC_NOWAIT;
	  if( semop(semid_waiting, &op, 1) == -1) // zwalniam blokade
	    printf("FAIL\n");
	}
      }
      exit(0);

    }
  }

  printf("Klienci stworzeni\n");
  int status, ret = 0;
  for( int i = 0; i < N; i++ ){
    wait(&status);
    if( WEXITSTATUS(status) == 1) ret = 1;
  }


  printf("Koniec strzyzen\n");
  return ret;

}

