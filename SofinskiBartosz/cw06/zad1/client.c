
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "msg.h"

#define FAIL(a) { perror(a); shut(1); }

FILE* f;
int my_id = -1;
int q = -1;

int startWith(const char* a, const char* b){
  while( a != NULL && *a != ' '){
    if( *a != *b) return 0;
    a++;
    b++;
  }
  return 1;
}

void shut(int r){

  msgctl(q, IPC_RMID, NULL);
  if( f != stdin) fclose(f);
  exit(r);
}
void shut0(){ shut(0); }


int main(int argc, char** argv){


  signal(SIGINT, shut0);

  f = stdin;
  if( argc == 2 ){
    f = fopen(argv[1], "r");
  }

  key_t k = ftok( getenv("$HOME"), 's');

  int sq = msgget(k, 0644);
  q = msgget(IPC_PRIVATE, 0644);

  if( sq == -1 ) FAIL("error at msgget -- server");
  if( q == -1 ) FAIL("error at msgget -- private");

  struct msg msg;
  msg.mtype = 1;
  memcpy( msg.buf, &q, sizeof(q));
  msg.id = getpid();
  char delim = ' ';

  if( msgsnd(sq, &msg, 68, 0) == -1) FAIL("error at msgsnd -- init");
  msgrcv( q, &msg, 68, 0, 0);
  char tmp[64];

  while( fgets( tmp, 68, f ) > 0 ){

    *index(tmp, '\n') = 0;
    char* op = strtok(tmp, &delim);
    msg.mtype = -1;
    if( strcmp(op, "MIRROR") == 0 ) msg.mtype = MIRROR;
    else if( strcmp(op, "TIME") == 0 ) msg.mtype = TIME;
    else if( strcmp(op, "CALC") == 0 ) msg.mtype = CALC;
    else if( strcmp(op, "END") == 0 ) msg.mtype = END;
    else if( strcmp(op, "STOP") == 0 ) msg.mtype = STOP;
    
    if( msg.mtype != -1 ){
      op = index(op, 0);
      op++;
      strcpy(msg.buf, op);
      msg.id = getpid();
      if( msgsnd(sq, &msg, 68, 0) == -1) FAIL("error at msgsnd -- loop");
      msgrcv(q, &msg, 68, 0, 0);
      printf("%s\n", msg.buf);
    }
    if ( msg.mtype == STOP ) shut(0);
  }

  shut(0);
}