
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include "msg.h"

int current_id = 0;
mqd_t client_qs[16];
mqd_t q;

#define FAIL(a) { perror(a); shut(1); }

void shut(int r){

  mq_close(q);
  mq_unlink("/serwer");

  exit(r);
}
void shut0(){ shut(0); }

int main(){

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 64;
  attr.mq_curmsgs = 0;

  q = mq_open("/serwer", O_RDWR | O_CREAT, 0644, &attr);
  if( q == (mqd_t)-1) FAIL("mq_open fails");

  signal(SIGINT, shut0);
  char msg[64];

  while( mq_receive(q, msg, 64, NULL) > 0){
    char id = msg[1];
    int client_q = -1;
    char output[MAX_BUF_SIZE];
    char* a = msg+2;
    char calc_ops[] = "+-*/";
    char* b;
    FILE* date_process;

    if ( id == -1 ){
      if (current_id < 16){
	// dodawanie nowego
	printf("Nowy klient o id: %d\n", current_id);
	if ( (client_q = mq_open(msg+2, O_RDWR, 0644, &attr)) == -1 ) FAIL("mq_open client_q fails:");

	client_qs[current_id] = client_q;
	msg[1] = current_id;
	mq_send( client_q, msg, MAX_BUF_SIZE, 0);

	current_id += 1;

      }
      continue;
    } else {
      client_q = client_qs[(int)id];
      printf("Msg od klienta o id: %d\n", id);
      printf("Typ: %d\n", msg[0]); // typ
      printf("Zawartosc: %s\n", msg+2); // msg
    }

    // faktyczne przetwarzanie zawartosci wiadomosci

    switch( msg[0] ){
    case MIRROR:
      b = index( a, 0 );
      int bsize = b-a;
      for(int j = 0; j < bsize; j++)
	output[j] = a[bsize-j-1];
      output[bsize] = 0;
      break;
    case TIME:

      date_process = popen("date +%H:%M:%S%n", "r");
      fread( output, MAX_BUF_SIZE, 1, date_process );
      pclose(date_process);

      *index( output, '\n') = 0;

      break;
    case CALC:

      b = strpbrk(a, calc_ops);
      if ( b == NULL){
	strcpy( msg+2, "error");
	mq_send( client_q, msg, MAX_BUF_SIZE, 0);
	continue;
      }
      char op = *b;
      *b = 0;
      b += 1;

      int x = strtol(a, NULL, 10);
      int y = strtol(b, NULL, 10);

      if( op == '+') x += y;
      else if ( op == '-' ) x -= y;
      else if ( op == '/' ) x /= y;
      else if ( op == '*' ) x *= y;

      sprintf(output, "%d", x);

      break;
    case END:
      attr.mq_flags = O_NONBLOCK;
      mq_setattr( q, &attr, NULL );
      break;
    case STOP:
      //mq_close( 
      //mq_unlink( client_q, IPC_RMID, NULL);
      break;
    }
    strcpy( msg+2, output );

    mq_send( client_q, msg, MAX_BUF_SIZE, 0);
  }

  if ( 0 ) FAIL("mq_receive fails");

  shut(0);
  return 0;
}

