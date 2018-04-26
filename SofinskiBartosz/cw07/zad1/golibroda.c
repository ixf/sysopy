#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "init_shared.h"

#define FAIL(msg) { perror(msg); exit(1); }

QueueDetails* q;
int * salon_queue;
extern int semid, semid_waiting, shmid, shmid_q;

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
  semop(semid, &op, 1);
  semop(semid_waiting, &op, 1);
  // domyslnie 1, czekamy na zero
  // 1 oznacza wolny fotel
  // 0 oznacza zajety + golibroda zarzadza fotelem i kolejka

  while(1){
    op.sem_op = 0;
    if ( semop(semid, &op, 1) == 0 ){
      printf("Jest klient!\n");
      // shm_seat jest zajete, kolejka nas na razie nie interesuje
      while(q->seat == 0){}
      client_pid = q->seat;

      do{

	sleep(1); // strzyzenie
	kill(client_pid, SIGRTMIN); // koniec strzyzenia
	client_pid = q->seat = 0;

	// wez potencjalne pid do strzyrzenia
	if( q->beg != q->end ){ // kiedy niepusta

	  client_pid = salon_queue[q->beg];
	  salon_queue[q->beg] = 0;
	  q->beg = ( q->beg + 1 ) % q->size;
	  kill(client_pid, SIGRTMIN); // zapros na fotel
	  q->seat = client_pid;
	}
      } while( client_pid != 0 );

      op.sem_op = 1;
      semop(semid, &op, 1); // fotel jest wolny

      op.sem_flg = IPC_NOWAIT;
    } else {
      // golibroda śpi
      op.sem_flg = 0;
      printf("Zasypiam...\n");
    }

  }

  return 0;
}

