
#include <stdlib.h>

#ifdef SORT_WITH_LIB

#include <stdio.h>
#define READ fread
#define SEEK fseek
#define WRITE fwrite
#define CLOSE fclose
#define OPEN fopen
#define FILETYPE FILE*
#define FILEMODE "r+"
#define SEEK_MODE_0 0
#define SEEK_MODE_1 1
int sort_lib(const char* source, int records, int record_length)

#else

#ifdef SORT_WITH_SYS

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define READ(BUF,SIZE,LEN,FILE) read(FILE,BUF,SIZE*LEN)
#define WRITE(BUF,SIZE,LEN,FILE) write(FILE,BUF,SIZE*LEN)
#define SEEK lseek
#define CLOSE close
#define OPEN open
#define FILETYPE int
#define FILEMODE O_RDWR
#define SEEK_MODE_0 SEEK_SET
#define SEEK_MODE_1 SEEK_CUR
int sort_sys(const char* source, int records, int record_length)

#endif
#endif
{

  unsigned char* other = malloc(sizeof(unsigned char) * record_length);
  unsigned char* key = malloc(sizeof(unsigned char) * record_length);
  FILETYPE f = OPEN(source, FILEMODE);

  // jestesmy na poczatku pliku
  // pierwszy odczyt powinien zostac wykonany na drugim ( indeks 1 ) rekordzie
  SEEK(f, record_length, SEEK_MODE_0);
      
  for(int i = 1; i < records; i++){
    SEEK(f, i*record_length, SEEK_MODE_0);
	  
    // wskazujemy na kolejny rekord
    READ(key, sizeof(unsigned char), record_length, f);

    // znajdujemy odpowiednie miejsce dla i-tego rekordu
    // wsrod pierwszych i-1 elementow tablicy rekordow
    // wstawiamy i-ty rekord na odpowiednie miejsce

    int j = i - 1;

    // uzywawm while true i breakow bo warunek na przerwanie petli
    // jest skomplikowany
    while(1){
      if( j < 0 )
	break;

      j -= 1;

      SEEK(f, (-2)*record_length, SEEK_MODE_1);

      READ(other, sizeof(unsigned char), record_length, f);

      if( other[0] < key[0]) { // obecna kolejnosc jest dobra
	break;
      } else { // trzeba zamienic

	// key jest w pamieci wiec mozna nadpisywac

	// jezeli nadpisujemy inny rekord to jest on zapisany w dwoch miejscach
	// a my nadpisujemy tylko jego lewa kopie


	SEEK(f, (-1)*record_length, SEEK_MODE_1);
	WRITE(key, sizeof(unsigned char), record_length, f);
	WRITE(other, sizeof(unsigned char), record_length, f);
	// powtarzamy
	SEEK(f, (-1)*record_length, SEEK_MODE_1);
      }
    }
  }

  return 0;
}
