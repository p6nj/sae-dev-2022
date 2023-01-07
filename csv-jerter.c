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
  int result; // various retrun value dispenser
  int server_fd, client_fd;
  struct sockaddr_in addr;
  FILE *file;

  // Create the server socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
    throw(1);

  // Bind the socket to a local address
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);
  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    throw(2);

  // Listen for incoming connections
  if (listen(server_fd, 1) < 0)
    throw(3);

  printf("Server listening on port %d...\n\x1B[32m", PORT);

  // Accept an incoming connection
  client_fd = accept(server_fd, NULL, NULL);
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
  result = fclose(file);
  if (result != 0)
    throw(6);
  result = close(client_fd);
  if (result != 0)
    throw(7);

  printf("Streams closed, goodbye!");

  return 0;
}