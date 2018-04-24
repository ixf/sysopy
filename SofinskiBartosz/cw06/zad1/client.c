
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "msg.h"

#define FAIL(a) { perror(a); exit(1); }

FILE* f;
int my_id = -1;
int sq, q = -1;

int startWith(const char* a, const char* b){
  while( a != NULL && *a != ' '){
    if( *a != *b) return 0;
    a++;
    b++;
  }
  return 1;
}

void shut(){

  struct msg msg;
  msg.mtype = STOP;
  msg.id = getpid();
  msgsnd(sq, &msg, MAX_TOTAL_SIZE, 0);

  msgctl(q, IPC_RMID, NULL);
  if( f != stdin ) fclose(f);
}

void sigintHandler(){ exit(0); }


int main(int argc, char** argv){

  atexit(shut);
  signal(SIGINT, sigintHandler);

  f = stdin;
  if( argc == 2 ){
    f = fopen(argv[1], "r");
  }

  key_t k = ftok( getenv("$HOME"), 's');

  sq = msgget(k, 0644);
  q = msgget(IPC_PRIVATE, 0644);

  if( sq == -1 ) FAIL("error at msgget -- server");
  if( q == -1 ) FAIL("error at msgget -- private");

  struct msg msg;
  msg.mtype = 1;
  memcpy( msg.buf, &q, sizeof(q));
  msg.id = getpid();
  char delim = ' ';

  if( msgsnd(sq, &msg, MAX_TOTAL_SIZE, 0) == -1) FAIL("error at msgsnd -- init");
  msgrcv( q, &msg, MAX_TOTAL_SIZE, 0, 0);
  char tmp[MAX_BUF_SIZE];

  while( fgets( tmp, MAX_TOTAL_SIZE, f ) > 0 ){

    *index(tmp, '\n') = 0;
    if( tmp[0] == 0 ) continue;
    char* op = strtok(tmp, &delim);
    msg.mtype = -1;
    if( strcmp(op, "MIRROR") == 0 ) msg.mtype = MIRROR;
    else if( strcmp(op, "TIME") == 0 ) msg.mtype = TIME;
    else if( strcmp(op, "CALC") == 0 ) msg.mtype = CALC;
    else if( strcmp(op, "END") == 0 ) msg.mtype = END;
    
    if( msg.mtype != -1 ){
      op = index(op, 0);
      op++;
      strcpy(msg.buf, op);
      msg.id = getpid();
      if( msgsnd(sq, &msg, MAX_TOTAL_SIZE, 0) == -1) FAIL("error at msgsnd -- loop");
      msgrcv(q, &msg, MAX_TOTAL_SIZE, 0, 0);
      printf("%s\n", msg.buf);
    }
    if ( msg.mtype == END ) exit(0);
  }

  exit(0);
}
