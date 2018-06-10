#pragma once

#define MAX_NAME_LENGTH 120

#define LEAVE(msg) { printf(msg); exit(1); }
#define FAIL(msg) { perror(msg); exit(2); }
#define TRY(F) { if( F != 0 ) LEAVE("ERROR AT " #F "\n") }
#define ETRY(F) { if( F != 0 ) FAIL("ERROR AT " #F ": ") }

typedef struct {
  char op;
  int id;
  int arg1, arg2;
} Message;


