#include <stdlib.h>

void test();
void* balloc(size_t size);
void bfree(void* mem);
void printEval();
void returnPages();
void bfreeTrackIntFrag(void* memory, size_t size);
