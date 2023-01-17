#include "fileops.c"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

// Creates and prepares a socket, then returns its file descriptor.
int sock()
{
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
int wait(int sockfd, struct sockaddr *cliaddr, int *clientfd)
{
  struct sockaddr_in a; // placeholder for sizeof
  unsigned int clilen = sizeof(a);
  *clientfd = accept(sockfd, cliaddr, &clilen);
  assertquit;
  return 1;
}

struct Result ask(int clientfd)
{
  struct Result r;
  char mode[2]; // read or write
  char *folder = "CSV";
  r = getdata(clientfd);
  assert;
  char prompt[strlen(r.string)];
  strcpy(prompt, r.string);
  check_filename(prompt);
  assert;
  return getfile(prompt, clientfd);
}

void respond(clientfd)
{
  struct Result r = ask(clientfd);
  char buffer[BUFFER_SIZE];
  char response[BUFFER_SIZE];
  sprintf(response, "%d", r.code);
  send(clientfd, response, BUFFER_SIZE, 0);
  strcpy(response, "");
  if (r.code != 0)
  {
    sprintf(response, r.string);
    send(clientfd, response, BUFFER_SIZE, 0);
    return;
  }
  // Read the file and send its contents to the client
  while (fgets(buffer, BUFFER_SIZE, r.file) != NULL)
    send(clientfd, buffer, strlen(buffer), 0);
}
