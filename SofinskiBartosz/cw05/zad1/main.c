#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define FAIL(text, ...) { fprintf(stderr, text, ##__VA_ARGS__); exit(1); }
#define PFAIL(text) { perror(text); exit(1); }

int main(int argc, char** argv){

  if(argc < 2) FAIL("not enough arguments\n");
  FILE* f = fopen(argv[1], "r");
  if( !f ) PFAIL("fopen fails");

  char* line = NULL;
  size_t n = 0;
  int status, code;
  char* args[16][16]; // wskazniki na kolejne argumenty kolejnych polecen w jednej linii
  // np. args[0][0] args[0][1] args[0][2]==NULL | args[1][0] args[1][1]==NULL | args[2][0] args[2][1] args[2][1] args[2][2]==NULL | args[3][0]==NULL

  while( getline(&line, &n, f) != -1 ){ 
    line[strlen(line)-1] = 0; // usuwanie \n
    int p = 0, a = 0; // proces, argument

    args[p][a] = strtok(line, " ");

    do {
      a += 1;
      args[p][a] = strtok(NULL, " ");
      if( args[p][a] != NULL){
	if( args[p][a][0] == '|' ){
	  args[p][a] = NULL;
	  p += 1;
	  a = 0;
	  args[p][a] = strtok(NULL, " ");
	} else if ( args[p][a][0] == '>' ){
	  p += 1;
	  args[p][0] = "dd";
	  args[p][1] = (char*)malloc(80);
	  strcpy(args[p][1], "of=");
	  char* tmp = strtok(NULL," ");
	  strcat(args[p][1], tmp);
	  args[p][2] = "status=none";

	  args[p][3] = NULL;
	  if( args[p-1][a][1] == '>' ){
	    args[p][3] = "conv=notrunc";
	    args[p][4] = "oflag=append";
	    args[p][5] = NULL;
	  }
	  args[p-1][a] = NULL;
	  p += 1;
	  a = 0;
	  args[p][a] = strtok(NULL, " ");
	}
      } 
    } while( args[p][a] != NULL );

    int pipes[2][2];
    // dla kazdego procesu wymagane sa dwa pipey
    // jeden do stdin poprzedniego procesu i drugi do stdout do kolejnego
    // w petli for od i do p zaczynamy od 0, czyli dla kazdego procesu
    // pipe do wejscia bedzie pipe[(i+1)%2)][0] ( zaczynajac od drugiego )
    // pipe do wyjscia bedzie pipe[i%2][1] ( poza ostatnim )

    for(int i = 0; i <= p; i++){
      // zamykam pipe ktory zaraz nadpiszemy
      // z tego pipe korzystal proces z petli i-2
      // na pewno skonczyl dzialanie bo mamy nizej wait(NULL)
      if(i > 1){
	close(pipes[i%2][0]);
	close(pipes[i%2][1]);
      }
      if( i < p && pipe(pipes[i%2]) < 0 ) PFAIL("pipe fails");

      if ( fork() == 0 ){ // child

	if( i > 0 ){
	  close(pipes[(i+1)%2][1]);
	  dup2(pipes[(i+1)%2][0], STDIN_FILENO);
	}
	if( i < p ){
	  close(pipes[i%2][0]);
	  dup2(pipes[i%2][1], STDOUT_FILENO);
	}

	execvp(args[i][0], args[i]);
	perror("exec error");
	exit(1);
      } else if(i > 0){ // dla kazdego poza pierwszym
	  wait(&status);
	  if( WIFEXITED(status) && (code = WEXITSTATUS(status)) != 0 )
	    FAIL("Error: %s has returned %d\n", args[i-1][0], code);
	}
    } 
    for(int x = 0; x < 4; x++)
	close(pipes[x/2][x%2]);

    // oczekuj na zamkniecie ostatniego procesu
    wait(&status);
    if( WIFEXITED(status) && (code = WEXITSTATUS(status)) != 0 )
      FAIL("Error: %s has returned %d\n", args[p][0], code);
  }
  free(line);
  fclose(f);
  return 0;
}
