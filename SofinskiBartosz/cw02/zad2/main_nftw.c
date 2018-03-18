
#define _XOPEN_SOURCE 500
#include <ftw.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <linux/limits.h>

int parse_dirs(const char *dirpath,
	       int (*fn) (const char *fpath, const struct stat *sb,
			  int typeflag, struct FTW *ftwbuf)) {

  DIR* d = opendir(dirpath);
  struct dirent* de;
  do{
    de = readdir(d);
    struct stat* buf;

    fn(de->d_name, buf, FTW_F, NULL);

  } while(de != NULL);

  closedir(d);
  return 0;
}

static struct tm pivot = {0};
static int cmp_result;

int cmp(time_t A, time_t B){
  return (A > B) ? 1 : ( A < B ) ? -1 : 0;
}

int tmcmp(struct tm* A, struct tm* B){
  int ret = cmp(A->tm_year, B->tm_year);
  if( ret ) return ret;
  ret = cmp(A->tm_mon, B->tm_mon);
  if( ret ) return ret;
  return cmp(A->tm_mday, B->tm_mday);
}

static int print_file_info(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){

  if( typeflag != FTW_F )
    return 0;

  int cmp;
  struct tm date;
  memcpy(&date, localtime(&(sb->st_mtime)), sizeof (struct tm));

  cmp = tmcmp(&pivot, &(date));

  if ( cmp != cmp_result )
    return 0;

  printf("-");
  int mode = sb->st_mode;
  for(int i = 0; i < 3; i++){
    int t = ( mode >> (6-i*3))%8;
    printf( (t & 4) ? "r" : "-");
    printf( (t & 2) ? "w" : "-");
    printf( (t & 1) ? "x" : "-");
  }

  printf(" %*ldB ", 8, sb->st_size );

  printf(" %ld ", sb->st_mtime );
  char* time_string = ctime(&(sb->st_mtime));
  printf(" %.24s ", time_string );

  char resolved_name[PATH_MAX+1];
  realpath(fpath, resolved_name);

  printf("%s\n", resolved_name);

  return 0;
}

int main(int argc, char** argv){

  if(argc < 4 ){
    printf("missing args\n");
    return -1;
  }

  if( strcmp("=", argv[2]) == 0){ // rowna
    cmp_result = 0;
  } else if( strcmp(">", argv[2]) == 0){ // pozniejsza
    cmp_result = 1;
  } else if( strcmp("<", argv[2]) == 0){ // wczesniejsza
    cmp_result = -1;
  } else {
    printf("wrong arg #2\n");
    return -2;
  }

  if ( strptime(argv[3], "%d-%m-%Y", &pivot) == NULL ){
    printf("Wrong time format in arg #3\n");
    return -3;
  }

#ifdef MAKE_WITH_NFTW
  nftw(argv[1], print_file_info, 1, FTW_PHYS);
#else
    #ifdef MAKE_WITH_DIRENT
  parse_dirs(argv[1], print_file_info);
    #else
	#error "Either MAKE_WITH_NFTW or MAKE_WITH_DIRENT must be defined"
    #endif
#endif

  return 0;
}