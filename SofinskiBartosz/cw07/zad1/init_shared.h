#pragma once

void shut();

typedef struct {
  int beg;
  int taken;
  int size;

  int seat; //pid
  int sleeping;
} Salon;

void init_shared(int flags);
void init_queue(int flags);

