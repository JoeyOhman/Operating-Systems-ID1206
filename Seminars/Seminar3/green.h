#include <ucontext.h>

typedef struct green_t {
  ucontext_t* context;
  void* (*fun)(void*);
  void *arg;
  struct green_t *next;
  struct green_t *join;
  int zombie;
} green_t;

int green_create(green_t* thread, void* (*fun)(void*), void* arg);
int green_yield();
int green_join(green_t* threaed);
