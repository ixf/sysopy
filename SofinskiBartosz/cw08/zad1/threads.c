#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

FILE* f;
void* func(void* x){
  char buf[128];
  if( fgets(buf, 128, f) > 0 ) {
    printf("%02d %s", *(int*)x, buf);
  }
  return NULL;
}

int main(){

  f = fopen("threads.c","r");
  pthread_t handle;
  int pnum = 50;
  for(int i = 0; i < pnum; i++){
    int* p = malloc(sizeof(int));
    *p = i;
    if( pthread_create(&handle, NULL, func, p) != 0)
      printf("failed\n");
  }

  void* r;
  for(int i = 0; i < pnum; i++)
    pthread_join(handle, &r);

}

