
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
int copy_lib(const char* source, const char* target, int records, int record_length)

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
#define FILEMODE_W O_WRONLY
int copy_sys(const char* source, const char* target, int records, int record_length)

#endif
#endif
{
  unsigned char* buf = malloc(sizeof(unsigned char) * record_length);
  FILETYPE sourcefile = OPEN(source, FILEMODE_R);
  FILETYPE targetfile = OPEN(source, FILEMODE_W);

  for(int i = 0; i < records; i++){
    READ(buf, sizeof(unsigned char), record_length, sourcefile);
    WRITE(buf, sizeof(unsigned char), record_length, targetfile);
  }

  CLOSE(sourcefile);
  CLOSE(targetfile);
  return 0;
}

