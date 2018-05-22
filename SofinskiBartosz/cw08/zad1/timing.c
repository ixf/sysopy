
#include "timing.h"
#include <time.h>
#include <stdio.h>

static struct timespec start_tp, end_tp;

void start_time(){
  clock_gettime(CLOCK_MONOTONIC, &start_tp);
}

void end_time(){
  clock_gettime(CLOCK_MONOTONIC, &end_tp);
  printf("Real time:\t%f\n",
      end_tp.tv_sec - start_tp.tv_sec +
      + ( end_tp.tv_nsec - start_tp.tv_nsec) / 1000000000.0);
}
