#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

char global[] = "This is a global string";
const int read_only = 123456;

void zot(unsigned long *stop) {
  unsigned long r = 0x3;

  unsigned long *i;
  for(i = &r; i <= stop; i++) {
    printf(" %p    0x%lx\n", i, *i);
  }
}

void foo(unsigned long *stop) {
  unsigned long q = 0x2;

  zot(stop);
}


int main() {

  int pid = getpid();

  unsigned long p = 0x1;

  foo(&p);

  char *heap = malloc(20);
  *heap = 0x61;
  printf("the heap variable at: %p\n", &heap);
  printf("pointing to: %p\n", heap);

  free(heap);
  char *foo = malloc(20);
  *foo = 0x62;
  *heap = 0x63;
  printf("foo after undef behavious: 0x%x\n", *foo);

  back:
    printf("\n\n /proc/%d/maps \n\n", pid);
    char command[50];

    sprintf(command, "cat /proc/%d/maps", pid);
    system(command);

    return 0;
}
