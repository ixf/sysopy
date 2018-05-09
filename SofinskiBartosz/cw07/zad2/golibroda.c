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

extern QueueDetails* q;
extern int * salon_queue;
extern sem_t* semid, *semid_sleep, *semid_waiting;
extern int shmid, shmid_q;

void exit0(){ exit(0); }

int main(int argc, char** argv){

  if (argc < 2){
    printf("Not enough args\n");
    exit(1);
  }

  atexit(shut);
  signal(SIGINT, exit0);

  int flags = O_CREAT;

  init_shared(flags);
  q->size = strtol(argv[1], NULL, 10);
  init_queue(q, flags);

  int client_pid;

  // domyslnie 1, czekamy na zero
  // 1 oznacza wolny fotel
  // 0 oznacza zajety + golibroda zarzadza fotelem i kolejka

  int nowait = 0; // flaga przez ktora wybieramy wait lub trywait

  while(1){

    int slept = (nowait) ? sem_trywait(semid_sleep) : sem_wait(semid_sleep);

    if ( slept == 0 ){
      printf("Jest klient!\n");
      // shm_seat jest zajete, kolejka nas na razie nie interesuje
      while(q->seat == 0){}
      client_pid = q->seat;

      do{

	//sleep(1); // strzyzenie
	kill(client_pid, SIGRTMIN); // koniec strzyzenia
	client_pid = q->seat = 0;

	// wez potencjalne pid do strzyrzenia
	if( q->taken > 0){ // kiedy niepusta

	  client_pid = salon_queue[q->beg];
	  salon_queue[q->beg] = 0;
	  q->beg = ( q->beg + 1 ) % q->size;
	  q->taken -= 1;
	  kill(client_pid, SIGRTMIN); // zapros na fotel
	  q->seat = client_pid;
	}
      } while( client_pid != 0 );

      sem_post(semid);

      nowait = 1;
    } else {
      // golibroda śpi
      nowait = 0;
      printf("Zasypiam...\n");
    }

  }

  return 0;
}

