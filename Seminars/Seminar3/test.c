#include <stdio.h>
#include "green.h"

void* test(void* arg) {
  int i = *(int*)arg;
  int loop = 4;
  while(loop > 0) {
    printf("thread %d: %d\n", i, loop);
    loop--;
    green_yield();
  }
}

void* testJoin(void* arg) {
  green_t* joined = (green_t*)arg;
  printf("joining...\n");
  green_join(joined);
  printf("joined!\n");
}

int main() {
  green_t g0, g1, g2, g3, g4;
  int a0 = 0;
  int a1 = 1;
  green_create(&g0, test, &a0);
  green_create(&g1, test, &a1);
  green_create(&g2, testJoin, &g1);
  green_create(&g3, testJoin, &g1);
  green_create(&g4, testJoin, &g1);

  green_join(&g1);
  green_join(&g0);
  green_join(&g2);
  green_join(&g3);
  green_join(&g4);
  printf("done\n");

  return 0;
}
