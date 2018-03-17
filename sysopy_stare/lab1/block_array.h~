
typedef enum { METHOD_STATIC=0, METHOD_DYNAMIC=1 } method;

char** create_block_array(int elems, int block_size, method m);
char* create_block(char** block_array, int elem, int block_size, method m);
void delete_block(char** block_array, int elem, method m);

int find_nearest(char** block_array, int elem, int elems, int block_size);
void delete_block_array(char** block_array, int elems);
