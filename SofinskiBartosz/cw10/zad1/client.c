
#define _DEFAULT_SOURCE
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <pthread.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <sys/epoll.h>

#include "shared.h"

int port;
int socketid = 0;
int method;
struct sockaddr_un uadr; // UNIX
struct sockaddr_in sadr; // IPv4
struct sockaddr* madr; // method address
char name[MAX_NAME_LENGTH];
char address[128];

void cleanup(){
  shutdown(socketid, SHUT_RDWR);
  close(socketid);
}

int main(int argc, char *argv[]){

  if(argc != 4)
    LEAVE("wrong arg number\n");

  strcpy(name, argv[1]);
  if( strcmp( argv[2], "UNIX" ) == 0)
    method = AF_UNIX;
  else if( strcmp( argv[2], "INET" ) == 0)
    method = AF_INET;
  else LEAVE("wrong method\n");
  strcpy(address, argv[3]);

  atexit(cleanup);

  if((socketid = socket(method, SOCK_STREAM, 0)) < 0) LEAVE("Error : Could not create socket \n");

  if( method == AF_INET ){
    char* sep_index = rindex(address, ':');
    *sep_index = 0;
    sadr.sin_family = AF_INET;
    sadr.sin_addr.s_addr = htonl(INADDR_ANY);
    sadr.sin_port = htons(strtol(sep_index+1, NULL, 10)); 
    if(inet_pton(AF_INET, address, &sadr.sin_addr)<=0)
      LEAVE("inet_pton error occured\n");

    if( connect(socketid, (struct sockaddr *)&sadr, sizeof(sadr)) < 0) FAIL("connect");
  } else {
    uadr.sun_family = AF_UNIX;
    strcpy(uadr.sun_path,address);

    if( connect(socketid, (struct sockaddr *)&uadr, sizeof(uadr)) < 0) FAIL("connect");
  }

  if( -1 == send(socketid, name, strlen(name)+1, 0) )
    LEAVE("send failed");

  char buf[128];
  Message m;

  while(1){
    if( recv(socketid, &buf, sizeof(buf), 0) > 0 ){
      if( strcmp(buf, "pls leave") == 0){
        printf("ok im leaving\n");
        exit(0);
      }
      if( strcmp(buf, "ping") == 0){
        printf("got ping\n");
        if( -1 == send(socketid, "pong", 5, 0) ) LEAVE("send failed");
        continue;
      }
      m = *(Message*)buf;
      printf("%d %c %d %d\n", m.id, m.op, m.arg1, m.arg2);
      int result;
      if( m.op == '+') result = m.arg1 + m.arg2;
      else if( m.op == '-') result = m.arg1 - m.arg2;
      else if( m.op == '*') result = m.arg1 * m.arg2;
      else if( m.op == '/' && m.arg2 != 0) result = m.arg1 / m.arg2;
      else {
        sprintf(buf, "Out[%d]: ERROR", m.id);
        if( -1 == send(socketid, buf, strlen(buf), 0) ) LEAVE("send failed");
        continue;
      }
      sprintf(buf, "Out[%d]: %d", m.id, result);
      if( -1 == send(socketid, buf, strlen(buf), 0) ) LEAVE("send failed");
    }
  }

  exit(0);
}

