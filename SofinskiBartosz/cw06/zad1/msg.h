#pragma once

struct msg{
  long mtype;
  int id;
  char buf[64];
};

enum STATMENT_TYPE { MIRROR=1, CALC, TIME, END, STOP };


