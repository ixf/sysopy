#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define FAIL(a) { perror(a); exit(1); }

int shm;
char* arr;

void shut(){
  munmap(arr, 64);
  shm_unlink("shmema");
}

void exit0() { exit(0); }

int main(){

  atexit(shut);
  signal(SIGINT, exit0);

  int s;
  if ( ( shm = shm_open("shmema", O_RDWR | O_CREAT, 0644) ) == -1 ) FAIL("shm_open fails");
  if ( ( s = ftruncate(shm, 64) ) == -1) FAIL("ftruncate fails");

  if ( ( arr = mmap(NULL, 64, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0) ) == (void*)-1) FAIL("mmap fails");

  char tmp[64];

  while(1){
    fgets(tmp, 64, stdin);
    printf("%s\n", arr);
    strcpy(arr, tmp);
  }

  shut();
  exit(0);
}

