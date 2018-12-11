#include <stdio.h>
#include "green.h"


int flag = 0;
green_cond_t cond;
green_mutex_t mutex;
int numThreads = 2;

int counter = 0;

void* testCV(void* arg) {
  int id = *(int*)arg;
  int loop = 4;
  while(loop > 0) {
    if(flag == id) {
      printf("thread %d: %d\n", id, loop);
      for(int i = 0; i < 1000000; i++) {
        if(i < 1)
          i++;
      }
      loop--;
      flag = (id + 1) % numThreads;
      green_cond_signal(&cond);
    } else {
      //printf("Thread waiting: %d, flag: %d\n", id, flag);
      green_cond_signal(&cond);
      green_cond_wait(&cond);
    }

  }
}

void* test(void* arg) {
  int id = *(int*)arg;
  int loop = 100000;
  while(loop > 0) {
    //if(loop % 1000 == 0)
      //printf("thread %d: %d\n", id, loop/1000);
    loop--;
    //green_yield();
  }
}

void* testJoin(void* arg) {
  green_t* joined = (green_t*)arg;
  printf("joining...\n");
  green_join(joined);
  printf("joined!\n");
  for(int i = 0; i < 10000; i++)
    if(i > 8)
      i++;
}

void* testSharedResource(void* arg) {
  int id = *(int*)arg;
  for(int i = 0; i < 1000000; i++) {
    green_mutex_lock(&mutex);
    int temp = counter;

    int dummy = 0;
    if(temp > 1337) // waste time between read and write
      dummy++;

    temp++;
    counter = temp;
    green_mutex_unlock(&mutex);
  }
}

int main() {
  green_cond_init(&cond);
  green_mutex_init(&mutex);
  for(int runs = 0; runs < 1; runs++) {
    green_t threads[numThreads];

    int args[numThreads];
    for(int i = 0; i < numThreads; i++)
      args[i] = i;

    for(int i = 0; i < numThreads; i++) {
      green_create(&threads[i], testSharedResource, &args[i]);
      //printf("Thread %d created!\n", i);
    }

    /*
    green_create(&threads[0], test, &args[0]);
    for(int i = 1; i < numThreads; i++)
      green_create(&threads[i], testJoin, &threads[0]);
    */

    for(int i = 0; i < numThreads; i++) {
      green_join(&threads[i]);
      //printf("Joined thread %d\n", i);
    }

  }
  printf("counter: %d\n", counter);
  printf("done\n");

  return 0;
}
