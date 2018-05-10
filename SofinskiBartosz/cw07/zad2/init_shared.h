#pragma once

void shut();

typedef struct {
  int beg;
  int taken;
  int size;

  int seat; //pid
  int sleeping;
} Salon;

typedef enum { sleeping = 0, cut = 1, leave = 2, enter_wr = 3} OPS;
// wake -> sleeping
// cut -> sit_down
// done -> leave
// leave_wr -> enter_wr

void init_shared(int flags);
void init_queue(int flags);

