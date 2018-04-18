
#include <mqueue.h>

#include <signal.h>

mqd_t q;

void shut(){

  mq_close(q);
  mq_unlink(q);

}

int main(){

  q = mq_open("serwer", O_RDWR | O_CREAT, 644, null );

  char echo_buf[64];

  while(1){
    if( mq_receive(q, echo_buf, 64, null) > 0 )
      printf("%s\n", echo_buf);
  }

  shut();
  return 0;
}


