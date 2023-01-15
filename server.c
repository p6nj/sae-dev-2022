#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include "common.c"
#include "src/server/fileops.c"
#include "src/server/netops.c"

int main()
{

  int sockfd = sock();
  struct sockaddr cliaddr;

  while (wait(sockfd, &cliaddr))
  {

    println("IP%s",
            inet_ntoa(((struct sockaddr_in *)&cliaddr)->sin_addr));

    // Read the filename from the client
    n = read(client_fd, buffer, BUFFER_SIZE);
    if (n < 0)
      NTHROW("reading filename from the client", 8);
    buffer[n] = '\0'; // remove last dumb char that causes problems
    sprintf(name, "%s", strtok(buffer, ""));
    n = strlen(name);
    if (!filename_ok(name))
    {
      char temp[strlen(name) + 40];
      sprintf(temp, "parsing file name (illegal filename)");
      NTHROW(temp, 403);
    }
    mode[0] = name[n - 1];
    if (mode[0] == 'w')
    {
      folder[0] = 'O';
    }
    name[n - 1] = '\0';
    sprintf(filename, "%s/%s.csv", folder, name);

    // Open the CSV file
    file = fopen(filename, mode);
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