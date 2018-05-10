#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "init_shared.h"

#define FAIL(msg) { perror(msg); exit(1); }

int client_pid;
extern volatile Salon* q;
extern volatile int * salon_queue;
extern struct sembuf sleeping, wake, sit_down, cut, leave, done, enter_wr, leave_wr;
extern int sem, shmid, shmid_q;
// waiting blokada na pamiec wspolna
// seat fotel do golenia + stan golenia

void exit0(){
  exit(0);
}

int main(int argc, char** argv){

  if (argc < 2){
    printf("Not enough args\n");
    exit(1);
  }

  atexit(shut);
  signal(SIGINT, exit0);

  init_shared(IPC_CREAT);
  q->size = strtol(argv[1], NULL, 10);
  init_queue(IPC_CREAT);

  semop(sem, &leave_wr, 1);

  while(1){

    if( semop(sem, &sleeping, 1) == 0 ){
      printf("Golibroda sie budzi\n");

      do{

	semop(sem, &cut, 1); // czekam az klient na pewno zajmie miejsce na fotelu
	client_pid = q->seat;
	printf("Rozpoczynam strzyrzenie %d\n", client_pid);
	client_pid = q->seat = 0;
	semop(sem, &done, 1);
	printf("Koniec strzyrzenia %d\n", client_pid);

	// wez potencjalne pid do strzyrzenia
	semop(sem, &enter_wr, 1);
	printf("%d %d %d\n", q->beg, q->taken, q->size);
	if( q->taken > 0){ // kiedy niepusta
	  client_pid = salon_queue[q->beg];
	  salon_queue[q->beg] = 0;
	  q->beg = ( q->beg + 1 ) % q->size;
	  q->taken -= 1;
	  printf("Zapraszam %d\n", client_pid);
	  kill(client_pid, SIGRTMIN); // zapros na fotel
	}

	if( client_pid == 0 )
	  q->sleeping = 1;
	semop(sem, &leave_wr, 1);

      } while( client_pid != 0 );

      // nie ma wiecej klientow
      printf("Golibroda zasypia\n");
    }
  }

  return 0;
}

