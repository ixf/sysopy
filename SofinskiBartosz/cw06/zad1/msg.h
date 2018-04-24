#pragma once

#define KEY_BYTE 's'
#define MAX_TOTAL_SIZE 68
#define MAX_BUF_SIZE 64

struct msg{
  long mtype;
  int id;
  char buf[MAX_BUF_SIZE];
};

enum STATMENT_TYPE { MIRROR=1, CALC, TIME, END, STOP };


