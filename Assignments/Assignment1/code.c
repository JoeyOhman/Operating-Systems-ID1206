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

  back:
    printf("\n\n /proc/%d/maps \n\n", pid);
    char command[50];

    sprintf(command, "cat /proc/%d/maps", pid);
    system(command);

    return 0;
}

/*
0x7fffffffe328    0x3                 zot: r
0x7fffffffe330    0x7fffffffe330      zot: i pointing to itself
0x7fffffffe338    0xdf541555aa680c00  unkown: fill to make it harder for hackers?
0x7fffffffe340    0x7fffffffe370      unkown: pointer to previous unknown?(stack)
0x7fffffffe348    0x55555555481c      foo: return address
0x7fffffffe350    0x0                 alignmentfill?
0x7fffffffe358    0x7fffffffe388      main: p
0x7fffffffe360    0x0                 alignmentfill?
0x7fffffffe368    0x2                 foo: q
0x7fffffffe370    0x7fffffffe3d0      unknown: stack pointer?
0x7fffffffe378    0x555555554852      main: return address
0x7fffffffe380    0xfcdffffe3e8       main: pid
0x7fffffffe388    0x1                 main: p
*/
