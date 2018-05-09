#pragma once

void shut();

typedef struct {
  int beg;
  int taken;
  int size;
  int seat; //pid
} QueueDetails;

QueueDetails* init_shared(int flags);
void init_queue(QueueDetails* q, int flags);

