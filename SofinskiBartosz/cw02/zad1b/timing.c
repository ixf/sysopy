
#include "timing.h"
#include <sys/times.h>
#include <stdio.h>

static struct tms start_tms, end_tms;

void print_times(){
  printf("User time:\t%011ld\n",  end_tms.tms_utime - start_tms.tms_utime);
  printf("System time:\t%011ld\n",  end_tms.tms_stime - start_tms.tms_stime);
}

void start_time(){
  times(&start_tms);
}

void end_time(){
  times(&end_tms);
  print_times();
}
