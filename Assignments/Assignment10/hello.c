#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <assert.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define TEST "Hello, hello"
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

#define SERVER "lower"
#define CLIENT "help"
#define MAX 512

int main(void) {
  int sock;
  char buffer[MAX];

  assert((sock = socket(AF_INET, SOCK_DGRAM, 0)) != -1);

  struct sockaddr_in name;
  name.sin_family = AF_INET;
  name.sin_port = 0;
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  assert(bind(sock, (struct sockaddr*) &name, sizeof(struct sockaddr_in)) != -1);

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  server.sin_addr.s_addr = inet_addr(SERVER_IP);

  int n = sizeof(TEST);
  int size = sizeof(struct sockaddr_in);

  assert(sendto(sock, TEST, n, 0, (struct sockaddr*) &server, sizeof(name)) != -1);
  n = recvfrom(sock, buffer, MAX-1, 0, (struct sockaddr*) &server, &size);
  if(n == -1)
    perror("server");

  buffer[n] = 0;
  printf("Client received: %s\n", buffer);
  unlink(CLIENT);
  exit(0);
}
