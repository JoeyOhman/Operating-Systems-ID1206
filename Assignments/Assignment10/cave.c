#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ITERATIONS 10
#define BURST 1000

int main() {

  // assert(0 == pipe(descr));
  int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  mkfifo("sesame", mode);

  int flag = O_WRONLY;
  int pipe = open("sesame", flag);

  // producer
  for(int i = 0; i < ITERATIONS; i++) {
    for(int j = 0; j < BURST; j++) {
      write(pipe, "0123456789", 10);
    }
    printf("producer burst %d done\n", i);
  }


  printf("producer done\n");
  //wait(NULL);
  //printf("all done\n");
}
