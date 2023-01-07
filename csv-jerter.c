#include "errors.c"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
  int sockfd, client_fd;
  unsigned int clilen;
  struct sockaddr_in serv_addr;
  struct sockaddr cli_addr;
  FILE *file;
  char buffer[BUFFER_SIZE];
  char filename[BUFFER_SIZE];

  // Create the server socket
  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    throw(1);

  // Bind the socket to a local address
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    throw(2);

  // Listen for n incoming connections
  if (listen(sockfd, 1) < 0)
    throw(3);

  printf("Server listening on port %d...\n\x1B[32m", PORT);

  // Wait for an incoming connection
  clilen = sizeof(serv_addr);
  client_fd = accept(sockfd, &cli_addr, &clilen);
  if (client_fd < 0)
    throw(4);

  printf("Client connected with IP %s!\n",
         inet_ntoa(((struct sockaddr_in *)&cli_addr)->sin_addr));

  // Read the filename from the client
  if (read(client_fd, buffer, BUFFER_SIZE) < 0)
    throw(8);
  buffer[strlen(buffer) - 2] =
      '\0'; // remove last dumb char that causes problems WITH TELNET
  sprintf(filename, "CSV/%s.csv", strtok(buffer, " "));

  // Open the CSV file
  file = fopen(filename, "r");
  if (file == NULL)
    throw(5);

  // Read the file and send its contents to the client
  while (fgets(buffer, BUFFER_SIZE, file) != NULL)
    send(client_fd, buffer, strlen(buffer), 0);

  printf("%s sent!\n", filename);

  // Close the file and the client socket
  if (fclose(file) != 0)
    throw(6);
  if (close(client_fd) != 0)
    throw(7);

  printf("Streams closed, goodbye!\n");

  return 0;
}