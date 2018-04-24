
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "msg.h"

#define FAIL(a) { perror(a); exit(0); }

mqd_t q, sq;
FILE* f;
char q_name[16];

void shut(){

  mq_close(sq);
  mq_close(q);
  mq_unlink(q_name);
}

void sigintHandler(){ exit(0); }

int main(int argc, char** argv){

  if( atexit( shut ) != 0 ) FAIL("atexit failed");

  signal(SIGINT, sigintHandler);
  srand(time(NULL));

  f = stdin;
  if( argc == 2 ){
    f = fopen(argv[1], "r");
  }

  for(int i = 1; i < 15; i++)
    q_name[i] = 'a' + rand()%26;
  q_name[0] = '/';
  q_name[15] = 0;

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 64;
  attr.mq_curmsgs = 0;

  q = mq_open(q_name, O_RDWR | O_CREAT, 0644, &attr );
  sq = mq_open("/serwer", O_RDWR, 0644, &attr);

  if( q == (mqd_t)-1) FAIL("mq_open fails -- client");
  if( sq == (mqd_t)-1 ) perror("mq_open fails -- server");

  char delim = ' ';
  char msg[64];
  msg[0] = 0;
  msg[1] = -1;
  strcpy( msg+2, q_name);

  if( mq_send(sq, msg, MAX_BUF_SIZE, 0) == -1) perror("mq_send fails:");
  if( mq_receive(q, msg, MAX_BUF_SIZE, 0) == -1) perror("mq_receive fails:");

  int my_id = msg[1];

  while( fgets( msg+2, MAX_BUF_SIZE-2, f ) > 0 ){

    *index(msg+2, '\n') = 0;
    if( msg[3] == 0 ) continue;
    char* op = strtok(msg+2, &delim);
    msg[0] = -1;
    if( strcmp(op, "MIRROR") == 0 ) msg[0] = MIRROR;
    else if( strcmp(op, "TIME") == 0 ) msg[0]= TIME;
    else if( strcmp(op, "CALC") == 0 ) msg[0]= CALC;
    else if( strcmp(op, "END") == 0 ) msg[0] = END;
    
    if( msg[0] != -1 ){
      op = index(op, 0) + 1;
      strcpy(msg+2, op);
      msg[1] = my_id;
      if( mq_send(sq, msg, MAX_BUF_SIZE, 0) == -1) FAIL("error at mq_send -- loop");
      if( mq_receive(q, msg, MAX_BUF_SIZE, 0) == -1) FAIL("error at mq_receive - loop");
      printf("%s\n", msg+2);
    }
    if ( msg[1] == END ) exit(0);
  }

  exit(0);
  return 0;
}

