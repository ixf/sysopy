#include "init_shared.h"
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

#define FAIL(msg) { perror(msg); exit(1); }

Salon* q;
sem_t* sem[8];
int shmid, shmid_q;
int* salon_queue;

char tmpname[5] = "/0sm";

void unmap(){
  for(int i = 0; i < 8; i+=1)
    sem_close(sem[i]);
  munmap(salon_queue, sizeof(int)*(q->size));
  munmap(q, sizeof(Salon));
}

void shut(){
  unmap();
  tmpname[1] = '0';
  for(int i = 0; i < 4; i+=1){
    sem_unlink(tmpname);
    tmpname[1] += 1;
  }
  shm_unlink("salon");
  shm_unlink("queue");
}

void init_shared(int flags){

  for(int i = 0; i < 4; i+=1){
    if((sem[i] = sem_open( tmpname, flags, 0644, 0 )) == SEM_FAILED) FAIL("sem_open fails -- sem");
    tmpname[1] += 1;
  }

  if ((shmid = shm_open("salon", O_RDWR | flags, 0644) ) == -1 ) FAIL("shm_open fails");
  if (ftruncate(shmid, sizeof(Salon)) == -1) FAIL("ftruncate fails");
  if ((q = mmap(NULL, sizeof(Salon), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0) ) == (void*)-1) FAIL("mmap fails");

  q->beg = 0;
  q->taken = 0;
  q->seat = 0;
}

void init_queue(int flags){
  if (( shmid_q = shm_open("queue", O_RDWR | flags, 0644) ) == -1 ) FAIL("shm_open fails");
  if (( ftruncate(shmid_q, sizeof(int)*(q->size)) ) == -1) FAIL("ftruncate fails");
  if (( salon_queue = mmap(NULL, sizeof(int)*(q->size), PROT_READ | PROT_WRITE, MAP_SHARED, shmid_q, 0) ) == (void*)-1) FAIL("mmap fails");
}

