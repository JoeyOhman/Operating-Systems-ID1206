#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
  int pid = fork(); int x = 3;

  if(pid == 0) {
    int fd = open("quotes.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    dup2(fd, 1);
    close(fd);
    execl("boba", "boba", NULL);
    printf("this will only happen if exec fails\n");
  } else {
    wait(NULL);
    //printf("We're done\n");
  }
  return 0;

}
