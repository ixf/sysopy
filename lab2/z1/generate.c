
#include <stdio.h>
#include <stdlib.h>

int generate(const char* source, int records, int record_length){
	      
    char* buf = malloc(record_length);
    FILE* f = fopen(source, "w");
    FILE* randomIn = fopen("/dev/urandom", "r");

    if(random == NULL){
      printf("Error at open(\"dev/urandom\")\n");
      return 1;
    }

    for(int i = 0; i < records; i++){
      fread(buf, sizeof(char), record_length, random);
      fwrite(buf, sizeof(char), record_length, f);
    }

    fclose(random);
    fclose(f);

    return 0;

}
