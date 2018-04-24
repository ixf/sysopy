
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include "msg.h"

#define FAIL(a) { perror(a); shut(1); }

int current_id = 0;
int client_pids[16];
int client_qs[16];
int q;

void shut(int r){

  msgctl(q, IPC_RMID, NULL);
  exit(r);
}
void shut0(){ shut(0); }


int main(int argc, char** argv){

  for(int i = 0; i < 16; i++){
    client_qs[i] = -1;
    client_pids[i] = -1;
  }

  signal(SIGINT, shut0);

  key_t k = ftok( getenv("$HOME"), 's');

  struct msg msg;
  msg.mtype = 1;
  int flags = 0; // IPC_NOWAIT
  q = msgget(k, IPC_CREAT | 0644);
  if( q == -1 ) FAIL("error at msgget");

  int bytes_read;
  while( (bytes_read = msgrcv(q, &msg, 68, 0, flags)) > 0 ){
    printf("%ld\n", msg.mtype);
    printf("%d\n", msg.id);
    printf("%s\n", msg.buf);

    int pid = msg.id;
    int client_q = -1;
    char output[64];
    char* a = msg.buf;
    char calc_ops[] = "+-*/";
    char* b;
    FILE* date_process;


    for(int i = 0; i < 16; i++){
      if( client_pids[i] == pid ){
	client_q = client_qs[i];
	break;
      }
    }

    if ( current_id < 16 && client_q == -1 ){
      // dodawanie nowego
      client_q = *(int*)msg.buf;
      client_qs[current_id] = client_q;
      client_pids[current_id] = pid;
      current_id += 1;
      msg.buf[0] = 0;
      msg.id = current_id;
    } else {

      // faktyczne przetwarzanie zawartosci wiadomosci

      switch( msg.mtype ){
      case MIRROR:

	b = index( a, 0 );
	int bsize = b-a;
	for(int j = 0; j < bsize; j++)
	  output[j] = a[bsize-j-1];
	output[bsize] = 0;
	break;
      case TIME:

	date_process = popen("date +%H:%M:%S%n", "r");
	fread( output, 64, 1, date_process );
	pclose(date_process);

	*index( output, '\n') = 0;

	break;
      case CALC:

	b = strpbrk(a, calc_ops);
	if ( b == NULL){
	  strcpy( msg.buf, "error");
	  msgsnd( client_q, &msg, 68, 0);
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
	flags = IPC_NOWAIT;
	break;
      case STOP:
	msgctl( client_q, IPC_RMID, NULL);
	break;
      }
      strcpy( msg.buf, output );
    }

    msgsnd( client_q, &msg, 68, 0);
  }

  if ( errno == ENOMSG ) shut(0);
  else FAIL("error at msgrcv");

}


