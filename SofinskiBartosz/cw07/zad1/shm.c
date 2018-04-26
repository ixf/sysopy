#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/shm.h>
#include <sys/ipc.h>

#define FAIL(a) { perror(a); exit(1); }

int shmid;
char* arr;

int main(){

  key_t k = ftok( getenv("$HOME"), 'a');
  int shmflg = 0;
  if( (shmid = shmget( k, 64, IPC_CREAT | 0644) ) == -1) FAIL("shmget fails");
  if( (arr = shmat(shmid, NULL, shmflg)) == (void*)-1) FAIL("smhat fails");

  char tmp[64];

  while(1){
    fgets(tmp, 64, stdin);
    printf("%s\n", arr);
    strcpy(arr, tmp);
  }


  exit(0);
}
