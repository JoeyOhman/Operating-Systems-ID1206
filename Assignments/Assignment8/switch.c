#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

int main() {

  int done = 0;
  ucontext_t one;
  ucontext_t two;

  getcontext(&one);

  printf("hello \n");

  if(!done) {
    // printf("Was not done!\n");
    done = 1;
    swapcontext(&two, &one);
  }

  return 0;
}
