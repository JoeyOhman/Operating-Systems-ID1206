#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

#define MAX 10

static int running;

static ucontext_t cntx_one;
static ucontext_t cntx_two;
static ucontext_t cntx_main;
static ucontext_t cntx_done;

static int done1;
static int done2;

void done() {
  int done = 0;

  while(!done) {
    if(running == 1) {

      printf("- process one terminating -\n");
      done1 = 1;
      if(! done2) {
        running = 2;
        swapcontext(&cntx_done, &cntx_two);
      } else {
        done = 1;
      }
    } else {

      printf("- process two terminating -\n");
      done2 = 1;
      if(!done1) {
        running = 1;
        swapcontext(&cntx_done, &cntx_one);
      } else {
        done = 1;
      }
    }
  }
  printf("- Done terminating -\n");
}

void yield() {
  printf("- yield -\n");
  if(running == 1) {
    running = 2;
    swapcontext(&cntx_one, &cntx_two);
  } else {
    running = 1;
    swapcontext(&cntx_two, &cntx_one);
  }
}

void push(int p, int i) {
  if(i < MAX) {
    printf("%d%*s push\n", p, i, " ");
    push(p, i+1);
    printf("%d%*s pop\n", p, i, " ");
  } else {
    printf("%d%*s top\n", p, i, " ");
    yield();
  }
}

int main() {

    char stack1[8*1024];
    char stack2[8*1024];
    char stack_done[8*1024];

    // The first context

    getcontext(&cntx_one);
    cntx_one.uc_stack.ss_sp = stack1;
    cntx_one.uc_stack.ss_size = sizeof(stack1);
    cntx_one.uc_link = &cntx_done;
    makecontext(&cntx_one, (void (*) (void)) push, 2, 1, 1);

    getcontext(&cntx_two);
    cntx_two.uc_stack.ss_sp = stack2;
    cntx_two.uc_stack.ss_size = sizeof(stack2);
    cntx_two.uc_link = &cntx_done;
    makecontext(&cntx_two, (void (*) (void)) push, 2, 2, 1);

    getcontext(&cntx_done);
    cntx_done.uc_link = &cntx_main;
    cntx_done.uc_stack.ss_sp = stack_done;
    cntx_done.uc_stack.ss_size = sizeof(stack_done);
    makecontext(&cntx_done, (void (*) (void)) done, 0);

    running = 1;

    printf("Let's go!\n");
    swapcontext(&cntx_main, &cntx_one);
    printf("contexts done!\n");
    return 0;
}
