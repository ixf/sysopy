
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

mqd_t q;

#define FAIL(a) { perror(a); shut(1); }

void shut(int r){

  mq_close(q);
  mq_unlink("/serwer");

  exit(r);
}

int main(){

  struct mq_attr a;
  a.mq_flags = 0;
  a.mq_maxmsg = 10;
  a.mq_msgsize = 64;
  a.mq_curmsgs = 0;

  q = mq_open("/serwer", O_RDWR | O_CREAT, (mode_t)0644, &a);
  if( q == (mqd_t)-1) FAIL("mq_open fails");

  signal(SIGINT, shut);
  char echo_buf[64];

  while(1){

    if( mq_receive(q, echo_buf, 64, NULL) == -1 ) FAIL("mq_receive fails");
    printf("%s\n", echo_buf);
    sleep(1);

  }

  shut(0);
  return 0;
}


