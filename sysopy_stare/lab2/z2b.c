#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>


static int print_file_info(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
  printf( typeflag == FTW_D ? "d " : typeflag == FTW_F ? "f " : "? ");
  printf("%s\n", fpath);
  return 0;
}

int main(){

  nftw(".", print_file_info, 1, 0);
  return 0;
  

}
