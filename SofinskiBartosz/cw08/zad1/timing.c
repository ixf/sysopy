
#include "timing.h"
#include <sys/times.h>
#include <stdio.h>

static struct tms start_tms, end_tms;

void start_time(){
  times(&start_tms);
}

void end_time(){
  times(&end_tms);
  printf("User time:\t%011ld\n",  end_tms.tms_utime - start_tms.tms_utime);
  printf("System time:\t%011ld\n",  end_tms.tms_stime - start_tms.tms_stime);
}
