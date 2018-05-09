#include "init_shared.h"
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

#define FAIL(msg) { perror(msg); exit(1); }

sem_t* semid, *semid_waiting, *semid_sleep;
int shmid, shmid_q;
int* salon_queue;
key_t k, k_q, k_waiting;
QueueDetails* q;

void unmap(){
  sem_close(semid);
  sem_close(semid_waiting);
  sem_close(semid_sleep);
  munmap(salon_queue, sizeof(int)*(q->size));
  munmap(q, sizeof(int)*4);
}

void shut(){
  unmap();
  sem_unlink("sleep");
  sem_unlink("seat");
  sem_unlink("waiting");
  shm_unlink("queue");
  shm_unlink("salon");
}

void init_shared(int flags){

  if((semid = sem_open( "seat", flags, 0644, 1 )) == SEM_FAILED) FAIL("sem_open fails -- semid");
  if((semid_sleep = sem_open( "sleep", flags, 0644, 0 )) == SEM_FAILED) FAIL("sem_open fails -- semid");
  if((semid_waiting = sem_open( "waiting", flags, 0644, 1)) == SEM_FAILED) FAIL("sem_open fails -- semid_waiting");

  if ((shmid = shm_open("salon", O_RDWR | flags, 0644) ) == -1 ) FAIL("shm_open fails");
  if (ftruncate(shmid, sizeof(int)*4) == -1) FAIL("ftruncate fails");
  if ((q = mmap(NULL, sizeof(int)*4, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0) ) == (void*)-1) FAIL("mmap fails");

  q->beg = 0;
  q->end = 0;
  q->seat = 0;
}

void init_queue(QueueDetails* q, int flags){
  if (( shmid_q = shm_open("queue", O_RDWR | flags, 0644) ) == -1 ) FAIL("shm_open fails");
  if (( ftruncate(shmid_q, sizeof(int)*(q->size)) ) == -1) FAIL("ftruncate fails");
  if (( salon_queue = mmap(NULL, sizeof(int)*(q->size), PROT_READ | PROT_WRITE, MAP_SHARED, shmid_q, 0) ) == (void*)-1) FAIL("mmap fails");
}

/*

void init_shared(int flags){

  key_t k = ftok( getenv("HOME"), 'g');
  key_t k_q = ftok( getenv("HOME"), 'q');
  key_t k_waiting = ftok( getenv("HOME"), 'w');
  if( k == -1) FAIL("ftok k fails");
  if( k_q == -1) FAIL("ftok k_q fails");
  if( k_waiting == -1) FAIL("ftok k_waiting fails");

  if( (semid = semget( k, 1, 0644 | flags )) == -1) FAIL("semget fails -- semid");
  if( (semid_waiting = semget( k_waiting, 1, 0644 | flags )) == -1) FAIL("semget fails -- semid_waiting");

  if ((shmid = shmget( k, sizeof(int)*4, flags | 0644)) == -1) FAIL("shmget fails -- shmid");
  if ((shm_array = shmat(shmid, NULL, 0)) == (void*)-1) FAIL("smhat fails -- shm_array");
  queue_beg = shm_array;
  queue_end = shm_array + sizeof(int);
  queue_size = shm_array + 2*sizeof(int);
  shm_seat = shm_array + 3*sizeof(int);

  *queue_beg = 0;
  *queue_end = 0;
  *shm_seat = 0;
}

void init_queue(int flags){

  if ((shmid_q = shmget( k_q, sizeof(int)*(*queue_size), 0644 | flags )) == -1) FAIL("shmget fails -- shmid_q");
  if ((salon_queue = shmat(shmid_q, NULL, 0)) == (void*)-1) FAIL("smhat fails -- salon_queue");

}

*/
