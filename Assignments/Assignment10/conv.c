#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <assert.h>

#define TEST "This is a TeSt to SEE IF it WoRKs"
#define SERVER "lower"
#define CLIENT "help"
#define MAX 512

int main(void) {
  int sock;
  char buffer[MAX];

  int size = sizeof(struct sockaddr_un);
  assert((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) != -1);

  struct sockaddr_un name = {AF_UNIX, CLIENT};

  assert(bind(sock, (struct sockaddr*) &name, size) != -1);

  struct sockaddr_un server = {AF_UNIX, SERVER};

  int n = sizeof(TEST);

  assert(sendto(sock, TEST, n, 0, (struct sockaddr*) &server, size) != -1);
  n = recvfrom(sock, buffer, MAX-1, 0, (struct sockaddr*) &server, &size);
  if(n == -1)
    perror("server");

  buffer[n] = 0;
  printf("Client received: %s\n", buffer);
  unlink(CLIENT);
  exit(0);
}
