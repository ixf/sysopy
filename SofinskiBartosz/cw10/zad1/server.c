
#define _DEFAULT_SOURCE
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
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

#define MAX_CLIENTS 16

int port;
int inetfd = 0, unixfd = 0;
char clients[MAX_CLIENTS][120];
int clientsfds[MAX_CLIENTS];
int client_index = -1;
struct sockaddr_un uadr; // SOCKET UNIX
struct sockaddr_in sadr; // SOCKET IPv4
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t msg_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

void add(char* msg, int fd){
  pthread_mutex_lock(&clients_mutex);
  if( client_index + 1 < MAX_CLIENTS ){
    client_index += 1;
    strcpy(clients[client_index], msg);
    clientsfds[client_index] = fd;
  }
  pthread_mutex_unlock(&clients_mutex);
}

void rem_fd(int fd){
  pthread_mutex_lock(&clients_mutex);
  int i;
  for(i = 0; i < MAX_CLIENTS; i++){
    if(clientsfds[i] == fd)
      break;
  }
  send(clientsfds[i], "pls leave", 10, 0);
  for(; i < MAX_CLIENTS-1; i++){
    memcpy(clients[i], clients[i+1], 120);
    clientsfds[i] = clientsfds[i+1];
  }
  client_index -= 1;
  pthread_mutex_unlock(&clients_mutex);
}

int exists(int fd){
  pthread_mutex_lock(&clients_mutex);
  for(int i = 0; i <= client_index; i++)
    if( clientsfds[i] == fd){
      pthread_mutex_unlock(&clients_mutex);
      return 1;
    }
  pthread_mutex_unlock(&clients_mutex);
  return 0;
}

int name_exists(char* name){
  pthread_mutex_lock(&clients_mutex);
  for(int i = 0; i <= client_index; i++)
    if(strcmp(name, clients[i]) == 0){
      pthread_mutex_unlock(&clients_mutex);
      return 1;
    }
  pthread_mutex_unlock(&clients_mutex);
  return 0;
}

void cleanup(){
  for(int i = 0; i <= client_index; i++){
    send(clientsfds[i], "pls leave", 10, 0);
    close(clientsfds[i]);
  }
  unlink(uadr.sun_path);
  close(unixfd);
  close(inetfd);
}

void cleanexit(){
  cleanup();
  exit(0);
}

void* thread_terminal(){

  char cmd[32];
  int id = 1;
  Message m;

  while(1){
    printf("In [%d]: ", id);
    scanf("%s", cmd);
    if( strcmp(cmd, "exit") == 0) break;
    if( client_index > -1 && sscanf(cmd, "%d %c %d", &m.arg1, &m.op, &m.arg2) == 3){
      m.id = id;
      int randindex = rand() % (client_index+1);
      int fd = clientsfds[randindex];
      if( -1 == send(fd, (void*)&m, sizeof(m), MSG_MORE) )
        LEAVE("send failed");
      pthread_mutex_lock(&print_mutex);
    } else {
      printf("Out[%d]: server-side error\n", id);
    }
    id += 1;
  }

  return NULL;
}

void* thread_network(){
  int epollfd = epoll_create1(0);
  struct epoll_event event;
  event.data.fd = unixfd;
  event.events = EPOLLIN; 
  epoll_ctl(epollfd, EPOLL_CTL_ADD, unixfd, &event);
  event.data.fd = inetfd;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, inetfd, &event);
  event.events = EPOLLIN;

  struct epoll_event events[MAX_CLIENTS];

  char msg[1024];

  int nfd = 0;
  struct sockaddr addr;
  socklen_t addr_len;

  while(1){

    int n = epoll_wait(epollfd, events, MAX_CLIENTS, -1);

    for( int i = 0; i < n; i++){
      memset(msg, 0, sizeof(msg));
      if( EPOLLIN == events[i].events){
          if( events[i].data.fd == inetfd || events[i].data.fd == unixfd ){
            if(( nfd = accept4(events[i].data.fd, &addr, &addr_len, SOCK_NONBLOCK) ) != -1 ){
              event.data.fd = nfd;
              epoll_ctl(epollfd, EPOLL_CTL_ADD, nfd, &event);
            }
          } else {
            int fd = events[i].data.fd;
            pthread_mutex_lock(&msg_mutex);
            if( recv(fd, msg, 1024, 0) > 0 ){
              if( ! exists(fd)  ){
                if ( ! name_exists(msg) )
                  add(msg, fd);
                else {
                  send(fd, "pls leave", 10, 0);
                  close(fd);
                }
              } else {
                printf("%s\n", msg);
                pthread_mutex_unlock(&print_mutex);
              }
            } else {
              if( errno != EAGAIN ){
                perror("recv");
                rem_fd(fd);
                epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event);
                close(fd);
              }
              pthread_mutex_unlock(&print_mutex);
            }
            pthread_mutex_unlock(&msg_mutex);
          }
      } else {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &event);
        rem_fd(events[i].data.fd);
        shutdown(events[i].data.fd, SHUT_RDWR);
        close(events[i].data.fd);
      }
    }
  }

  return NULL;
}

void* thread_pingpong(){

  int epollfd = epoll_create1(0);
  struct epoll_event event;
  event.data.fd = unixfd;
  event.events = EPOLLIN;

  char response[8];
  while(1){
    sleep(1);
    pthread_mutex_lock(&msg_mutex);
    for(int i = 0; i <= client_index; i++){
      int fd = clientsfds[i];
      epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
      int ok = 0;
      do {
        send(fd, "ping", 5, MSG_MORE);
        int n = epoll_wait(epollfd, &event, MAX_CLIENTS, 2000);
        for(int j = 0; j < n; j++){
          if( recv(fd, response, 8, 0) > 0){
            if( strcmp(response, "pong") == 0){
              epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event);
              ok = 2;
              break;
            } else {
              ok = 1;
              printf("%s\n", response);
            }
          }
        }
      } while( ok == 1 );
      if( ok == 0 ){
        printf("deleting %d\n", i);
        rem_fd(fd);
        shutdown(fd, SHUT_RDWR);
        close(fd);
        i -= 1;
      }
    }
    pthread_mutex_unlock(&msg_mutex);
  }
}

void pass(){}

int main(int argc, char *argv[]) {

  srand(time(NULL));

  if( argc < 2 ) LEAVE("Not enough args\n");

  port = strtol(argv[1], NULL, 10);

  atexit(cleanup);
  signal(SIGINT, cleanexit);
  signal(SIGPIPE, pass);
  pthread_mutex_lock(&print_mutex);
  
  uadr.sun_family = AF_UNIX;
  strcpy(uadr.sun_path,argv[2]);

  sadr.sin_family = AF_INET;
  sadr.sin_addr.s_addr = htonl(INADDR_ANY);
  sadr.sin_port = htons(port); 

  inetfd = socket(AF_INET, SOCK_STREAM, 0);
  unixfd = socket(AF_UNIX, SOCK_STREAM, 0);

  unlink(uadr.sun_path);
  ETRY( bind(unixfd, (struct sockaddr*)&uadr, sizeof(uadr)));
  ETRY( bind(inetfd, (struct sockaddr*)&sadr, sizeof(sadr)));

  TRY( listen(inetfd, MAX_CLIENTS) );
  TRY( listen(unixfd, MAX_CLIENTS) );

  pthread_t network, pingpong;
  pthread_create(&network, NULL, thread_network, NULL);
  pthread_create(&pingpong, NULL, thread_pingpong, NULL);
  thread_terminal();
  pthread_join(pingpong, NULL);
  pthread_join(network, NULL);
  
  exit(0);
}
