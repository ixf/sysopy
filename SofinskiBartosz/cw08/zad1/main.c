#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>

#include "timing.h"

#define LEAVE(msg) { printf(msg); exit(1); }
#define FAIL(msg) { perror(msg); exit(2); }

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define CLAMP(a,b,c) MAX((a),MIN((b),(c)))

unsigned char** I; // picture. I[y][x] ( tablica wierszy )
float** K; // filter matrix
unsigned char** J; // wyjscie

int N, M, C; // wymiary
int threads;

void* splot(void* thread_num_param){
  int thread_num = *(int*)thread_num_param;

  double s;

  for(int x = N*thread_num/threads; x < N*(thread_num+1)/threads; x++){
    for(int y = 0; y < M; y++){

      s = 0;
      for (int i = 0; i < C; i++ ){
        for (int j = 0; j < C; j++ ){
          int index_x = CLAMP(0, x-(int)ceil(C/2)+i, N-1);
          int index_y = CLAMP(0, y-(int)ceil(C/2)+j, M-1);
          s += I[index_y][index_x] * K[j][i];
        }
      }
      J[y][x] = round(s);
    }
  }

  return NULL;
}


int main(int argc, char** argv){

  if( argc < 5 ) LEAVE("Not enough args\n");

  threads = strtol(argv[1], NULL, 10);
  char* picture_filename = argv[2]; 
  char* filter_filename = argv[3]; 
  char* output_filename = argv[4]; 

  FILE* f;
  if( (f = fopen(picture_filename, "r")) == NULL) FAIL("fopen");
  char word[8];
  int val;
  float fval;

  // czytanie z pliku wejsciowego
  if( fscanf(f, "%s%*[ \n\t]", word) < 1 || strcmp("P2", word) != 0)
    LEAVE("Wrong picture format (1)\n");

  if( fscanf(f, "%d %d\n%d\n", &N, &M, &val) < 1 || val != 255)
    LEAVE("Wrong picture format (2)\n");

  I = malloc(M * sizeof(char*));
  J = malloc(M * sizeof(char*));
  for(int y = 0; y < M; y++){
    I[y] = malloc(N * sizeof(char));
    J[y] = malloc(N * sizeof(char));
    for(int x = 0; x < N; x++){
      fscanf(f, "%d%*[ \n]", &val);
      I[y][x] = val;
    }
  }
  fclose(f);

  // czytanie z pliku filtru
  if( (f = fopen(filter_filename, "r")) == NULL) FAIL("fopen");

  if(fscanf(f, "%d\n", &C) < 1)
    LEAVE("Wrong filter format\n");

  K = malloc(C * sizeof(float*));
  for(int y = 0; y < C; y++){
    K[y] = malloc(C * sizeof(float));
    for(int x = 0; x < C; x++){
      fscanf(f, "%f%*[ \n]", &fval);
      K[y][x] = fval;
    }
  }
  fclose(f);

  // watki i filtrowanie

  pthread_t* handles = malloc(threads * sizeof(pthread_t));

  start_time(); // funkcja z pliku timing.c ( dziala na zmiennych statycznych )

  for(int i = 0; i < threads; i++){
    int* param = malloc(sizeof(int));
    *param = i;
    pthread_create(&handles[i], NULL, splot, param);
  }

  for(int i = 0; i < threads; i++){
    void* x;
    pthread_join(handles[i], &x);
  }

  end_time();

  // wpis do pliku wyjsciowego
  if( (f = fopen(output_filename, "w")) == NULL) FAIL("fopen");

  fprintf(f, "P2\n%d %d\n255\n", N, M);

  for(int y = 0; y < M; y++){
    for(int x = 0; x < N; x++){
      fprintf(f, "%d\n", J[y][x]);
    }
  }
  fclose(f);

}

