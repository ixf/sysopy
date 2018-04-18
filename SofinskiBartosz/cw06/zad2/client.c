
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

mqd_t q;

void shut(){

  mq_close(q);
  mq_unlink(q);

}

int main(){

  q = mq_open("klient", O_RDWR | O_CREAT, 644, NULL );

  char got[64] = "ayyy\n";

  mqd_t sq = mq_open("serwer", O_RDWR);
  mq_send(sq, got, 64, NULL);

  shut();
  return 0;
}

