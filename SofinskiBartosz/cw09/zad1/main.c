#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define LEAVE(msg) { printf(msg); exit(1); }
#define FAIL(msg) { perror(msg); exit(2); }


char** buffer;
int last_produced, last_consumed;
int quit_flag = 0;
pthread_t* handles;
pthread_mutex_t* bmutex;
pthread_mutexattr_t bmutex_attr;
pthread_cond_t cond_produced = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_consumed = PTHREAD_COND_INITIALIZER;
pthread_condattr_t condattr;

FILE* f;
int P, K, N, L, nk;
char search_mode;
int print_all;

int strlen_unicode(char* str){
  int v = 0;
  while( *str != '\n' && *str != 0 ){
    if(( 0xC0 & *str) != 0x80 ){
      v += 1;
    }
    str += 1;
  }
  return v;
}

void handle_alarm(){
  pthread_mutex_lock(&bmutex[N]);
  quit_flag = 1;
  pthread_cond_broadcast(&cond_produced);
  pthread_mutex_unlock(&bmutex[N]);
}

void* produce(){

  while(1){

    // obliczanie miejsca i oczekiwanie

    pthread_mutex_lock(&bmutex[N]);
    if( quit_flag ){
      pthread_mutex_unlock(&bmutex[N]);
      pthread_exit(0);
    }
    if( print_all ) printf("PRODUCTION: lock buffer_pos\n");
    while( last_produced - last_consumed == N ){ // pelne
      if( print_all ) printf("PRODUCTION: full\n");
      if( quit_flag ){
        pthread_mutex_unlock(&bmutex[N]);
        pthread_exit(0);
      }
      pthread_cond_wait(&cond_consumed, &bmutex[N]);
    }
    last_produced = last_produced+1;
    int buffer_pos = last_produced%N;
    pthread_mutex_lock(&bmutex[buffer_pos]);
    pthread_cond_signal(&cond_produced);
    if( print_all ) printf("PRODUCTION: index=%d\n", buffer_pos);
    if( print_all ) printf("PRODUCTION: unlock buffer_pos\n");
    pthread_mutex_unlock(&bmutex[N]);

    // dzialanie

    buffer[buffer_pos] = malloc(sizeof(char)*1024);
    if( fgets(buffer[buffer_pos], 1024, f) == NULL){
      quit_flag = 1;
      pthread_cond_broadcast(&cond_produced);
      pthread_mutex_unlock(&bmutex[buffer_pos]);
      if( print_all ) printf("PRODUCTION: quitting\n");
      pthread_exit(0);
    }
    pthread_mutex_unlock(&bmutex[buffer_pos]);
  }

  return NULL;
}

void* consume(){

  while(1){

    // obliczanie miejsca w buforze i oczekiwanie
    
    pthread_mutex_lock(&bmutex[N]);
    if( print_all ) printf("CONSUMPTION: lock buffer_pos\n");
    while( last_consumed == last_produced ){ // puste
      if( print_all ) printf("CONSUMPTION: empty\n");
      if( quit_flag ){
        if( print_all ) printf("CONSUMPTION: quitting\n");
        pthread_mutex_unlock(&bmutex[N]);
        pthread_exit(0);
      }
      pthread_cond_wait(&cond_produced, &bmutex[N]);
    }
    last_consumed = last_consumed+1;
    int buffer_pos = last_consumed%N;
    pthread_mutex_lock(&bmutex[buffer_pos]);
    pthread_cond_signal(&cond_consumed);
    if( print_all ) printf("CONSUMPTION: index=%d\n", buffer_pos);
    if( print_all ) printf("CONSUMPTION: unlock buffer_pos\n");
    pthread_mutex_unlock(&bmutex[N]);

    // dzialanie

    int len = (int)strlen_unicode(buffer[buffer_pos]);
    if( ( search_mode == '>' && len > L ) || 
        ( search_mode == '=' && len == L ) || 
        ( search_mode == '<' && len < L )){
      printf("%d: %s", buffer_pos, buffer[buffer_pos]);
    }

    free(buffer[buffer_pos]);
    pthread_mutex_unlock(&bmutex[buffer_pos]);
  }
  return NULL;
}

int main(int argc, char** argv){

  if(argc < 2) LEAVE("Not enough arguments\n");
  char* config_file = argv[1];
  char filename[FILENAME_MAX];
  if( (f = fopen(config_file, "r")) == NULL) FAIL("fopen 1");
  fscanf(f, "%d %d %d %s %d %c %d %d", &P, &K, &N, filename, &L,
      &search_mode, &print_all, &nk);
  if(P < 1 || K < 1 || N < 1 || L < 1 || nk < 0 ||
      ( print_all != 0 && print_all != 1 ) ||
      ( search_mode != '>' && search_mode != '=' && search_mode != '<' ) ) LEAVE("wrong args");
  fclose(f);

  if( (f = fopen(filename, "r")) == NULL) FAIL("fopen 2");

  last_consumed = -1;
  last_produced = -1;

  signal(SIGALRM, handle_alarm);
  if(nk != 0) alarm(nk);

  buffer = calloc(sizeof(char*), N);
  bmutex = calloc(sizeof(pthread_mutex_t), N+1);
  for(int i = 0; i < N+1; i++){
    pthread_mutex_init(&bmutex[i], &bmutex_attr);
  }
  pthread_condattr_init(&condattr);

  
  handles = malloc((P+K)*sizeof(pthread_t));

  for(int i = 0; i < P+K; i++){
    void* param = malloc(sizeof(int));
    *(int*)param = i;

    if(i < P)
      pthread_create(&handles[i], NULL, produce, param);
    else
      pthread_create(&handles[i], NULL, consume, param);
  }

  for(int i = 0; i < P+K; i++){
    void* retval = NULL;
    pthread_join(handles[i], &retval);
  }
  fclose(f);

  pthread_condattr_destroy(&condattr);
  for(int i = 0; i < N+1; i++)
    pthread_mutex_destroy(&bmutex[i]);

  free(handles);
  free(buffer);
  free(bmutex);
  exit(0);
}

