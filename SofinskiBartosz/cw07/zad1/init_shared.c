#include "init_shared.h"
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define FAIL(msg) { perror(msg); exit(1); }

int semid, semid_waiting, semid_seat, shmid, shmid_q;
int* salon_queue;
key_t k, k_q, k_waiting, k_seat;

void shut(){
  if(semctl(semid, 0, IPC_RMID) == -1) perror("semctl fails");
  if(semctl(semid_waiting, 0, IPC_RMID) == -1) perror("semctl fails");
  if(semctl(semid_seat, 0, IPC_RMID) == -1) perror("semctl fails");
  if(shmctl(shmid, IPC_RMID, NULL) == -1) perror("shmctl fails");
  if(shmctl(shmid_q, IPC_RMID, NULL) == -1) perror("shmctl fails");
}

QueueDetails* init_shared(int flags){

  k = ftok( getenv("HOME"), 'g');
  k_q = ftok( getenv("HOME"), 'q');
  k_waiting = ftok( getenv("HOME"), 'w');
  k_seat = ftok( getenv("HOME"), 's');
  if( k == -1) FAIL("ftok k fails");
  if( k_q == -1) FAIL("ftok k_q fails");
  if( k_waiting == -1) FAIL("ftok k_waiting fails");
  if( k_seat == -1) FAIL("ftok k_seat fails");

  if( (semid = semget( k, 1, 0644 | flags )) == -1) FAIL("semget fails -- semid");
  if( (semid_waiting = semget( k_waiting, 1, 0644 | flags )) == -1) FAIL("semget fails -- semid_waiting");
  if( (semid_seat = semget( k_seat , 1, 0644 | flags )) == -1) FAIL("semget fails -- semid_seat");
  printf("semid_seat: %d\n", semctl(semid_seat, 0, GETVAL));

  QueueDetails* q;
  if ((shmid = shmget( k, sizeof(int)*4, flags | 0644)) == -1) FAIL("shmget fails -- shmid");
  if ((q = shmat(shmid, NULL, 0)) == (void*)-1) FAIL("smhat fails -- shm_array");
  /* q->beg = shm_array; */
  /* q->end = shm_array + sizeof(int); */
  /* q->size = shm_array + 2*sizeof(int); */
  /* q->seat = shm_array + 3*sizeof(int); */

  q->beg = 0;
  q->taken = 0;
  q->seat = 0;
  return q;
}

void init_queue(QueueDetails* q, int flags){
  if ((shmid_q = shmget( k_q, sizeof(int)*(q->size), 0644 | flags )) == -1) FAIL("shmget fails -- shmid_q");
  if ((salon_queue = shmat(shmid_q, NULL, 0)) == (void*)-1) FAIL("smhat fails -- salon_queue");
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
