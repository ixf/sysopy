#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

#include "init_shared.h"

#define FAIL(msg) { perror(msg); exit(1); }

extern Salon* q;
extern int * salon_queue;
extern sem_t* sem[8];

void exit0(){ exit(0); }

int main(int argc, char** argv){

  if (argc < 2){
    printf("Not enough args\n");
    exit(1);
  }

  atexit(shut);
  signal(SIGINT, exit0);
  signal(SIGTERM, exit0);

  init_shared(O_CREAT);
  q->size = strtol(argv[1], NULL, 10);
  init_queue(O_CREAT);

  int client_pid;
  sem_post(sem[enter_wr]);
  q->sleeping = 1;

  while(1){

    if ( sem_wait(sem[sleeping]) == 0 ){
      printf("Golibroda sie budzi\n");

      do{

	sem_wait(sem[cut]);
	client_pid = q->seat;
	printf("Rozpoczynam strzyrzenie %d\n", client_pid);
	sem_post(sem[leave]);
	printf("Koniec strzyrzenia %d\n", client_pid);
	client_pid = q->seat = 0;

	// wez potencjalne pid do strzyrzenia
	sem_wait(sem[enter_wr]);
	if( q->taken > 0){ // kiedy niepusta
	  client_pid = salon_queue[q->beg];
	  salon_queue[q->beg] = 0;
	  q->beg = ( q->beg + 1 ) % q->size;
	  q->taken -= 1;
	  printf("Zapraszam %d\n", client_pid);
	  kill(client_pid, SIGRTMIN); // zapros na fotel
	  q->seat = client_pid;
	}

	if( client_pid == 0 )
	  q->sleeping = 1;
	sem_post(sem[enter_wr]);

      } while( client_pid != 0 );

      printf("Golibroda zasypia\n");
    }

  }

  return 0;
}

