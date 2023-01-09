#include "errors.c"
#include "preferences.c"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#define THROW(desc, code) return throw(desc, code);
#define NTHROW(desc, code)                                                     \
  {                                                                            \
    close(sockfd);                                                             \
    return throw(desc, code);                                                  \
  }

int main() {
  int sockfd, client_fd, n, optval;
  unsigned int clilen;
  struct sockaddr_in serv_addr;
  struct sockaddr cli_addr;
  FILE *file;
  char buffer[BUFFER_SIZE];
  char filename[BUFFER_SIZE];

  // Create the server socket
  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    THROW("creating socket", 1);

  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

  // Bind the socket to a local address
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    NTHROW("binding socket", 2);

  // Listen for n incoming connections
  if (listen(sockfd, 1) < 0)
    NTHROW("listening for connections", 3);

  printf("Server listening on port %d...\n\x1B[32m", PORT);

  // Wait for an incoming connection
  clilen = sizeof(serv_addr);
  while (1) {
    client_fd = accept(sockfd, &cli_addr, &clilen);
    if (client_fd < 0)
      NTHROW("accepting connection", 4);

    printf("Client connected with IP %s!\n",
           inet_ntoa(((struct sockaddr_in *)&cli_addr)->sin_addr));

    // Read the filename from the client
    n = read(client_fd, buffer, BUFFER_SIZE);
    if (n < 0)
      NTHROW("reading filename from the client", 8);
    buffer[n] = '\0'; // remove last dumb char that causes problems WITH TELNET
    sprintf(filename, "CSV/%s.csv", strtok(buffer, " "));

    // Open the CSV file
    file = fopen(filename, "r");
    if (file == NULL)
      NTHROW("opening file", 5);

    // Read the file and send its contents to the client
    while (fgets(buffer, BUFFER_SIZE, file) != NULL)
      send(client_fd, buffer, strlen(buffer), 0);

    printf("%s sent!\n", filename);

    // Empty the buffer and close the connection
    bzero(buffer, BUFFER_SIZE);
    if (close(client_fd) != 0)
      NTHROW("closing file descriptor", 7);
  }

  // Close the file and the client socket
  if (fclose(file) != 0)
    return throw("closing file stream", 6);
  if (close(client_fd) != 0)
    return throw("closing file descriptor", 7);
  close(sockfd);

  printf("Streams closed, goodbye!\n");

  return 0;
}
