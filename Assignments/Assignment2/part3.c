#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
  int pid = fork();

  if(pid == 0) {
    int child = getpid();
    printf("I'm the child %d in the group %d\n", child, getpgid(child));
  } else {
    int parent = getpid();
    printf("I'm the parent %d in the group %d\n", parent, getpgid(parent));
    wait(NULL);
  }
  return 0;

}
