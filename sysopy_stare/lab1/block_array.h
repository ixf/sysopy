
char** create_block_array_dynamic(int elems, int block_size);
void create_block_dynamic(char** block_array, int elem, int block_size);
void create_block_static(int elem);
void delete_block_dynamic(char** block_array, int elem, int block_size);
void delete_block_static(int elem);
void delete_block_array_dynamic(char** block_array, int elems);
int find_nearest_dynamic(char** block_array, int elem, int elems, int block_size);
int find_nearest_static(int elem);
