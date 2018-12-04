#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <assert.h>
#include <ctype.h>

#define SERVER "lower"
#define MAX 512

int main(void) {
  int sock;
  char buffer[MAX];

  assert((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) != -1);

  struct sockaddr_un name = {AF_UNIX, SERVER};

  assert(bind(sock, (struct sockaddr*) &name, sizeof(struct sockaddr_un)) != -1);


  struct sockaddr_un client;
  int size = sizeof(struct sockaddr_un);

  while(1) {
    int n;
    n = recvfrom(sock, buffer, MAX-1, 0, (struct sockaddr*) &client, &size);
    if(n == -1)
      perror("server");

    buffer[n] = 0;

    printf("Server received %s\n", buffer);

    for(int i = 0; i < n; i++)
      buffer[i] = tolower((unsigned char) buffer[i]);

      assert(sendto(sock, buffer, n , 0, (struct sockaddr*) &client, size) == n);
  }
}
