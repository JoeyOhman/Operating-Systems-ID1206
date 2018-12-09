#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <assert.h>
#include "green.h"

#define FALSE 0
#define TRUE 1

#define STACK_SIZE 4096

static ucontext_t main_cntx = {0};
static green_t main_green = {&main_cntx, NULL, NULL, NULL, NULL, FALSE};

static green_t* running = &main_green;

static void init() __attribute__((constructor));

void green_thread();
void add_to_ready_queue(green_t* thread);
green_t* pop_ready_queue();

void init() {
  getcontext(&main_cntx);
}

int green_create(green_t* new, void* (*fun)(void*), void* arg) {
  ucontext_t* cntx = (ucontext_t*) malloc(sizeof(ucontext_t));
  getcontext(cntx);

  void* stack = malloc(STACK_SIZE);

  cntx->uc_stack.ss_sp = stack;
  cntx->uc_stack.ss_size = STACK_SIZE;

  makecontext(cntx, green_thread, 0);
  new->context = cntx;
  new->fun = fun;
  new->arg = arg;
  new->next = NULL;
  new->join = NULL;
  new->zombie = FALSE;

  // add new to the ready queue
  add_to_ready_queue(new);

  return 0;
}

void green_thread() {
  green_t* this = running;

  // The actual workload of the thread
  (*this->fun)(this->arg);
  // Thread workload done, clean up

  // place joining thread in rdy queue
  if(this->join != NULL)
    add_to_ready_queue(this->join);

  // free allocated memory structures
  free(this->context->uc_stack.ss_sp);
  free(this->context);

  // we're a zombie
  this->zombie = TRUE;

  // find the next thread to run
  green_t* next = pop_ready_queue();
  // Now we are setting running->next to NULL as well, not needed if not reusing threads?

  running = next;
  setcontext(next->context); // Thread's life ends here (RIP)

}

int green_yield() {
  green_t* susp = running;
  // add susp to ready queue
  add_to_ready_queue(susp);
  // select the next thread for execution
  green_t* next = pop_ready_queue();

  running = next;
  swapcontext(susp->context, next->context); // Save this context to susp and continue from next
}

int green_join(green_t* thread) {
  if(thread->zombie)
    return 0;

  green_t* susp = running;
  // add to waiting threads
  if(thread->join == NULL) {
    thread->join = susp;
  } else {
    green_t* current = thread->join;
    while(current->next != NULL)
      current = current->next;
      
    current->next = susp;
  }

  // select the next thread for execution
  green_t* next = pop_ready_queue();

  running = next;
  swapcontext(susp->context, next->context);
  return 0;
}

// Put thread last in queue
void add_to_ready_queue(green_t* ready) {
  green_t* current = running;
  while(current->next != NULL)
    current = current->next;

  current->next = ready;
}

green_t* pop_ready_queue() {
  green_t* next = running->next;
  running->next = NULL;
  return next;
}
