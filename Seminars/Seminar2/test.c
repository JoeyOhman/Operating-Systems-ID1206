#include "buddy.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX 100 // Max size of block
#define MIN 8 // Min size of block
#define BUFFER 1000 // How many blocks we can hold
#define ALLOCS 100000 // How many times we alloc (and possibly free)

// Request smaller blocks far more often so simulate real behaviour of user
// size = MAX / e^r, where e^r [1, MAX/MIN] => size [MIN, MAX]
int request() {
  if(MIN == MAX) // To work properly for singe target values with same performance
    return MIN;
  // k is log(MAX/MIN)
  double k = log(((double) MAX) / MIN);
  // r is [0, k]
  double r = ((double)(rand() % (int)(k*10000))) / 10000;
  int size = (int)((double) MAX / exp(r));

  return size;
}

void benchSizeUtil(int bufferSize) {
  void *buffer[bufferSize]; // Initialize buffer
  size_t allocSize[bufferSize];
  for(int i = 0; i < bufferSize; i++) {
    buffer[i] = NULL;
    allocSize[i] = 0;
  }

  for(int i = 0; i < ALLOCS; i++) {
    // Find random index of buffer, free it if taken and alloc new
    int index = rand() % bufferSize;
    if(buffer[index] != NULL) {
      //bfree(buffer[index]);
      bfreeTrackIntFrag(buffer[index], allocSize[index]);
    }
    size_t size = (size_t)request();
    // printf("Requested size: %ld\n", size);
    int* memory;
    memory = balloc(size);

    if(memory == NULL) {
      fprintf(stderr, "mem alloc failed\n");
      return;
    }
    buffer[index] = memory;
    allocSize[index] = size;
    *memory = 123; // Actually writing to it, so pages really are allocated and used
  }
  printEval(bufferSize);
  for(int i = 0; i < bufferSize; i++) {
    if(buffer[i] != NULL)
      bfree(buffer[i]);
  }
  returnPages();
}


void evalMemory() {
  printf("# Evaluation of memory \n#\n#{BufferSize\tOSAllocated\tUserGiven\tUserUsed}\n");
  for(int i = 10; i < BUFFER; i+=10) {
    benchSizeUtil(i);
  }
  benchSizeUtil(BUFFER);
}

void evalTime() {
  int size_max = 4000;
  int numOps = 100000;
  int stride = 10;
  // int ROUNDS = 1;
  clock_t c_startBalloc, c_stopBalloc, c_startBfree, c_stopBfree;
  printf("# Evaluation of time performance \n#\n#{AllocSize(Byte)\tTime(ms)Balloc\tTime(ms)Bfree}\n");
  double processTimeBalloc = 0, processTimeBfree = 0;
  for(int size = stride; size <= size_max; size+=stride) {

    void* buffer[numOps];

    c_startBalloc = clock();
    for(int i = 0; i < numOps; i++) {
      //balloc((size_t)request());
      buffer[i] = malloc(size);
    }
    c_stopBalloc = clock();
    processTimeBalloc = ((double)(c_stopBalloc - c_startBalloc)) / ((double)CLOCKS_PER_SEC/1000);

    c_startBfree = clock();
    for(int i = 0; i < numOps; i++) {
      free(buffer[i]);
    }
    c_stopBfree = clock();
    processTimeBfree = ((double)(c_stopBfree - c_startBfree)) / ((double)CLOCKS_PER_SEC/1000);

    double avgOpTimeBalloc = (processTimeBalloc)/numOps;
    double avgOpTimeBfree = (processTimeBfree)/numOps;
    printf("%d\t%f\t%f\n", size, avgOpTimeBalloc, avgOpTimeBfree);
    // printf("Average time per balloc: %f\n", processTime/numOps);
    returnPages();
  }
}

int main() {
  //test();
  srand(time(NULL));

  evalMemory();

  //evalTime();

  printf("# Benchmark done!\n");
}
