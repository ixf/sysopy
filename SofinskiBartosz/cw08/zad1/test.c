
#include <pthread.h>
#include <string.h>


void* pass(void * arg){

}


int main(){

  for(int i = 0; i < threads; i++){
    int* param = malloc(sizeof(int));
    *param = i;
    pthread_create(&handles[i], NULL, splot, param);
  }

}
