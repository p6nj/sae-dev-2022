#include "errors.c"
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
  struct sockaddr_in serv_addr;
  FILE *file;

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
  client_fd = accept(sockfd, NULL, NULL);
  if (client_fd < 0)
    throw(4);

  printf("Client connected!\n");

  // Open the CSV file
  file = fopen("CSV/RT2FA.csv", "r");
  if (file == NULL)
    throw(5);

  // Read the file and send its contents to the client
  char buffer[BUFFER_SIZE];
  while (fgets(buffer, BUFFER_SIZE, file) != NULL)
    send(client_fd, buffer, strlen(buffer), 0);

  printf("Data sent!\n");

  // Close the file and the client socket
  if (fclose(file) != 0)
    throw(6);
  if (close(client_fd) != 0)
    throw(7);

  printf("Streams closed, goodbye!\n");

  return 0;
}