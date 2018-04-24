
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define FAIL(a) { perror(a); shut(1); }

mqd_t q, sq;

void shut(){

  mq_close(q);
  mq_unlink("/klient");

  exit(0);
}

int main(){

  q = mq_open("/klient", O_RDWR | O_CREAT, (mode_t)644, NULL );
  if( q == (mqd_t)-1) FAIL("mq_open fails");

  char got[64] = "ayyy\n";

  sq = mq_open("/serwer", O_WRONLY);
  if( sq == (mqd_t)-1 ) perror("mq_open fails");
  if( mq_send(sq, got, 64, 1) == -1) perror("mq_send fails:");
  mq_close(sq);

  shut();
  return 0;
}

