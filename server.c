// Structure of every connection: one request, one response.
// File name for edited CSV documents: [original file name]-[year]-[month]-[day]-[hour]-[duration]

#include "errors.c"
#include "preferences.c"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#define SUCCESS true
#define EPIC_SUCCESS SUCCESS
#define FAILURE false
#define EPIC_FAIL FAILURE
#define NTHROW(desc, code)    \
  {                           \
    close(sockfd);            \
    return throw(desc, code); \
  }

extern int errno;

// enum to use with the event struct for logging purposes
enum events
{
  ClientConnected,
  ReadFile,
  WriteFile,
  ClientDisconnected,
  ServerStarted
};

// struct of events to log with useful information
struct event
{
  // from `events` enum
  unsigned int code;
  // The event may have failed (FAILURE) or succeeded (SUCCESS); for unexpected status, please use EPIC_SUCCESS or EPIC_FAIL instead.
  // If status = false, `errno` will be used for error description on top of `message`.
  bool status;
  // IPv4 of the client responsible of the event if applicable
  struct sockaddr ip;
  // the name of the file being read or written if applicable
  char filename[BUFFER_SIZE];
};

/* Checks if a filename is right for the project format.
 * This will prevent clients from writting somewhere else than the provided
 * folder using either '..' or '/'.
 * On Windows you also have to check for '\'.
 * The filename is considered right only if it's just a name (no extension).
 */
bool filename_ok(char filename[])
{
  switch (filename[0])
  {
  case '.':
    return false;
  case '/':
    return false;
  default:;
  }
  if (strchr(filename, '.') != NULL)
    return false;
  if (strchr(filename, '/') != NULL)
    return false;
  // the access mode must be present at the end and can either be read or write
  switch (filename[strlen(filename) - 1])
  {
  case 'w':
    return true;
  case 'r':
    return true;
  }
  return false;
}

int main()
{
  int sockfd, client_fd, n, optval;
  unsigned int clilen;
  struct sockaddr_in serv_addr;
  struct sockaddr cli_addr;
  FILE *file;
  char buffer[BUFFER_SIZE], filename[BUFFER_SIZE];
  char mode[2];          // access mode (read or write) for the destination file
  char folder[] = "CSV"; // name of the folder to save or read file
  char name[BUFFER_SIZE];

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
  mode[1] = '\0';
  while (1)
  {
    client_fd = accept(sockfd, &cli_addr, &clilen);
    if (client_fd < 0)
      NTHROW("accepting connection", 4);

    printf("Client connected with IP %s!\n",
           inet_ntoa(((struct sockaddr_in *)&cli_addr)->sin_addr));

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
