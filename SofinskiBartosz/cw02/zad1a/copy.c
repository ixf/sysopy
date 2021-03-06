
#include <stdlib.h>

#ifdef MAKE_WITH_LIB

#include <stdio.h>
#define READ fread
#define WRITE fwrite
#define CLOSE fclose
#define OPEN fopen
#define FILETYPE FILE*
#define FILEMODE_R "r"
#define FILEMODE_W "w"
#define COPY copy_lib

#else

#ifdef MAKE_WITH_SYS

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define READ(BUF,SIZE,LEN,FILE) read(FILE,BUF,SIZE*LEN)
#define WRITE(BUF,SIZE,LEN,FILE) write(FILE,BUF,SIZE*LEN)
#define CLOSE close
#define OPEN open
#define FILETYPE int
#define FILEMODE_R O_RDONLY
#define FILEMODE_W O_WRONLY | O_CREAT | O_TRUNC, 0644
#define COPY copy_sys

#endif
#endif
int COPY(const char* source, const char* target, int records, int record_length) {
  unsigned char* buf = malloc(sizeof(unsigned char) * record_length);
  FILETYPE sourcefile = OPEN(source, FILEMODE_R);
  FILETYPE targetfile = OPEN(target, FILEMODE_W);

  for(int i = 0; i < records; i++){
    READ(buf, sizeof(unsigned char), record_length, sourcefile);
    WRITE(buf, sizeof(unsigned char), record_length, targetfile);
  }

  CLOSE(sourcefile);
  CLOSE(targetfile);

  free(buf);
  return 0;
}

