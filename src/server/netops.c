#include "../../common.c"
#include "fileops.c"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

// Creates and prepares a socket, then returns its file descriptor.
int sock() {
  int sockfd, optval;
  struct sockaddr_in srvaddr;
  struct Result r;

  // Create the server socket
  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  assertquit;

  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

  // Bind the socket to a local address
  srvaddr.sin_family = AF_INET;
  srvaddr.sin_addr.s_addr = INADDR_ANY;
  srvaddr.sin_port = htons(PORT);
  bind(sockfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr));
  assertquit;

  // Listen for n incoming connections
  listen(sockfd, 1);
  assertquit;

  println("Server listening on port %d...\x1B[32m", PORT);

  return sockfd;
}

// Wait for a connection, fill cliaddr with the client address and return 1.
int wait(int sockfd, struct sockaddr *cliaddr, int *clientfd) {
  struct sockaddr_in a; // placeholder for sizeof
  unsigned int clilen = sizeof(a);
  *clientfd = accept(sockfd, cliaddr, &clilen);
  assertquit;
  return 1;
}

struct Result ask(clientfd) {
  struct Result r;
  char mode[2]; // read or write
  char *folder = "CSV";
  char prompt[BUFFER_SIZE], buffer[BUFFER_SIZE];
  ssize_t n = read(clientfd, buffer, BUFFER_SIZE);
  assert;
  buffer[n] = '\0'; // remove last dumb char that causes problems
  sprintf(prompt, "%s", strtok(buffer, ""));
  // n = strlen(prompt);
  check_filename(prompt);
  assert;
  return action(prompt);
}

void respond(clientfd) {
  struct Result r = ask(clientfd);
  char buffer[BUFFER_SIZE];
  char response[strlen(r.string) + 5];
  sprintf(response, "%d\n%s", r.code, r.string);
  int i, j;
  for (i = 0; i < strlen(response) - BUFFER_SIZE; i += BUFFER_SIZE) {
    j = BUFFER_SIZE;
    while (--j)
      buffer[j] = response[i + j];
    send(clientfd, buffer, BUFFER_SIZE, 0);
  }
}