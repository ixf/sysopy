#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

void func(void* x){
  printf("%d\n", *(int*)x);
}

int main(){

  int x = 2;
  pthread_t handle;
  if( pthread_create(&handle, NULL, func, &x) != 0)
    printf("failed\n");

  sleep(1);

}

