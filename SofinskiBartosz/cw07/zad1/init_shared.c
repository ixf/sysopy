#include "init_shared.h"
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define FAIL(msg) { perror(msg); exit(1); }

volatile Salon* q;
volatile int * salon_queue;
int sem, shmid, shmid_q;
struct sembuf sleeping, wake, sit_down, cut, leave, done, enter_wr, leave_wr;
key_t k, k_q;
// sem:
// 0 modyfikacja kolejki + sprawdzanie stanu golibrody
// 1 sen golibrody
// 2 gotowosc klienta do strzyzenia
// 3 koniec strzyzenia

void shut(){
  if(semctl(sem, 0, IPC_RMID) == -1) perror("semctl fails seat");
  if(shmctl(shmid, IPC_RMID, NULL) == -1) perror("shmctl fails");
  if(shmctl(shmid_q, IPC_RMID, NULL) == -1) perror("shmctl fails");
}

void init_shared(int flags){
  enter_wr.sem_num = leave_wr.sem_num = 0;
  sleeping.sem_num = wake.sem_num = 1;
  sit_down.sem_num = cut.sem_num = 2;
  leave.sem_num = done.sem_num = 3;

  sleeping.sem_op = cut.sem_op = leave.sem_op = enter_wr.sem_op = -1;
  wake.sem_op = sit_down.sem_op = done.sem_op = leave_wr.sem_op = 1;

  wake.sem_flg = sleeping.sem_flg = cut.sem_flg = leave.sem_flg = enter_wr.sem_flg = sit_down.sem_flg = done.sem_flg = leave_wr.sem_flg = 0;


  k = ftok( getenv("HOME"), 'k');
  k_q = ftok( getenv("HOME"), 'q');
  if( k == -1) FAIL("ftok k_seat fails");
  if( k_q == -1) FAIL("ftok k_seat fails");

  if( (sem = semget( k, 4, 0644 | flags )) == -1) FAIL("semget fails -- details");
  if ((shmid = shmget( k, sizeof(Salon), flags | 0644)) == -1) FAIL("shmget fails -- shmid");
  if ((q = shmat(shmid, NULL, 0)) == (void*)-1) FAIL("smhat fails -- shm_array");

  q->beg = 0;
  q->taken = 0;
  q->seat = 0;
  q->sleeping = 1;
}

void init_queue(int flags){
  if ((shmid_q = shmget( k_q, sizeof(int)*(q->size), 0644 | flags )) == -1) FAIL("shmget fails -- shmid_q");
  if ((salon_queue = shmat(shmid_q, NULL, 0)) == (void*)-1) FAIL("smhat fails -- salon_queue");
}

/*

void init_shared(int flags){

  key_t k = ftok( getenv("HOME"), 'g');
  key_t k_q = ftok( getenv("HOME"), 'q');
  key_t k_details = ftok( getenv("HOME"), 'w');
  if( k == -1) FAIL("ftok k fails");
  if( k_q == -1) FAIL("ftok k_q fails");
  if( k_details == -1) FAIL("ftok k_details fails");

  if( (sen = semget( k, 1, 0644 | flags )) == -1) FAIL("semget fails -- sen");
  if( (details = semget( k_details, 1, 0644 | flags )) == -1) FAIL("semget fails -- details");

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
