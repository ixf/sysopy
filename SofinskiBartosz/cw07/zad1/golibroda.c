#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "init_shared.h"

#define FAIL(msg) { perror(msg); exit(1); }

QueueDetails* q;
int * salon_queue;
extern int semid, semid_waiting, semid_seat, shmid, shmid_q;
// semid stan golibrody.
// waiting blokada na kolejke
// seat fotel do golenia


void exit0(){ exit(0); }

int main(int argc, char** argv){

  if (argc < 2){
    printf("Not enough args\n");
    exit(1);
  }

  atexit(shut);
  signal(SIGINT, exit0);

  int flags = IPC_CREAT;

  struct sembuf op;
  op.sem_num = 0;
  op.sem_flg = 0;

  q = init_shared(flags);
  q->size = strtol(argv[1], NULL, 10);
  init_queue(q, flags);

  int client_pid;

  op.sem_op = 1;
  //semop(semid, &op, 1);
  semop(semid_waiting, &op, 1);
  // semop(semid_seat, &op, 1);
  // domyslnie 1, czekamy na zero
  // 1 oznacza wolny fotel
  // 0 oznacza zajety + golibroda zarzadza fotelem


  while(1){
    op.sem_op = -1;
    if( semop(semid, &op, 1) == 0 ){
      printf("Golibroda sie budzi\n");

      do{

	printf("Stan seat: %d\n", semctl(semid_seat, 0, GETVAL));
	op.sem_flg = 0;
	op.sem_op = -1;
	semop(semid_seat, &op, 1); // czekam az klient na pewno zajmie miejsce na fotelu
	client_pid = q->seat;
	printf("Rozpoczynam strzyrzenie %d\n", client_pid);
	sleep(1); // strzyzenie
	op.sem_op = 2;
	semop(semid_seat, &op, 1); // koniec strzyzenia
	printf("Koniec strzyrzenia %d\n", client_pid);
	client_pid = q->seat = 0;

	// wez potencjalne pid do strzyrzenia
	op.sem_op = -1;
	op.sem_flg = 0;
	semop(semid_waiting, &op, 1);
	if( q->taken > 0){ // kiedy niepusta
	  client_pid = salon_queue[q->beg];
	  salon_queue[q->beg] = 0;
	  q->beg = ( q->beg + 1 ) % q->size;
	  q->taken -= 1;
	  printf("Zapraszam %d\n", client_pid);
	  kill(client_pid, SIGRTMIN); // zapros na fotel
	}
	op.sem_op = 1;
	semop(semid_waiting, &op, 1);

      } while( client_pid != 0 );

      // nie ma wiecej klientow

      op.sem_flg = IPC_NOWAIT;
    } else {
      op.sem_flg = 0;
      printf("Golibroda zasypia\n");
    }
  }

  return 0;
}

