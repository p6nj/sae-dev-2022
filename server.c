// Structure of every connection: one request, one response.
// File name for edited CSV documents: [original file name]-[year]-[month]-[day]-[hour]-[duration]

#include "errors.c"
#include "preferences.c"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#define SUCCESS true
#define EPIC_SUCCESS SUCCESS
#define FAILURE false
#define EPIC_FAIL FAILURE
#define CLIENTCONNECTED           "CC %s", inet_ntoa(((struct sockaddr_in*) &ip)->sin_addr)
#define CLIENTDISCONNECTED        "CD %s", inet_ntoa(((struct sockaddr_in*) &ip)->sin_addr)
#define READFILE                  "RF %s %s", filename, inet_ntoa(((struct sockaddr_in*) &ip)->sin_addr)
#define WRITEFILE                 "WF %s %s", filename, inet_ntoa(((struct sockaddr_in*) &ip)->sin_addr)
#define SERVERSTARTED             "SS %d", PORT
#define fmt(f) sprintf(format, f);break
#define NTHROW(desc, code)    \
  {                           \
    close(sckfd);            \
    return throw(desc, code); \
  }


extern int errno;

// enum to use with the event struct for logging purposes
enum events {
  ClientConnected,
  ReadFile,
  WriteFile,
  ClientDisconnected,
  ServerStarted
};

void logg(
  // from `events` enum
  unsigned int event,
  // The event may have failed (FAILURE) or succeeded (SUCCESS); for unexpected status, please use EPIC_SUCCESS or EPIC_FAIL instead.
   // If status = false, `errno` will be used for error description on top of `message`.
  bool status,
  // IPv4 of the client responsible of the event if applicable
  struct sockaddr ip,
  // the name of the file being read or written if applicable
  char filename[BUFFER_SIZE]
) {
  char format[BUFFER_SIZE];
  // I didn't find anything more optimised than this piece of junk, behold:
  switch (event) {
    case ClientConnected:fmt(CLIENTCONNECTED);
    case ClientDisconnected:fmt(CLIENTDISCONNECTED);
    case ReadFile:fmt(READFILE);
    case WriteFile:fmt(WRITEFILE);
    case ServerStarted:fmt(SERVERSTARTED);
  }
  FILE* f = fopen("log.txt", "a+");
  fprintf(f, "%lu %s %s\n", (unsigned long) time(NULL), status ? "✅" : "❎", format);
  fclose(f);
}

/* Checks if a filename is right for the project format.
 * This will prevent clients from writting somewhere else than the provided
 * folder using either '..' or '/'.
 * On Windows you also have to check for '\'.
 * The filename is considered right only if it's just a name (no extension).
 */
bool filename_ok(char filename[]) {
  switch (filename[0]) {
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
  switch (filename[strlen(filename) - 1]) {
    case 'w':
      return true;
    case 'r':
      return true;
  }
  return false;
}

int main() {
  int sckfd, clifd, n, optval;
  unsigned int iplen;
  struct sockaddr_in srvaddr;
  struct sockaddr cliaddr;
  FILE* file;
  char buffer[BUFFER_SIZE], filename[BUFFER_SIZE];
  char mode[2];          // access mode (read or write) for the destination file
  char folder[] = "CSV"; // name of the folder to save or read file
  char name[BUFFER_SIZE];

  // Create the server socket
  sckfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sckfd < 0)
    THROW("creating socket", 1);

  optval = 1;
  setsockopt(sckfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

  // Bind the socket to a local address
  srvaddr.sin_family = AF_INET;
  srvaddr.sin_addr.s_addr = INADDR_ANY;
  srvaddr.sin_port = htons(PORT);
  if (bind(sckfd, (struct sockaddr*) &srvaddr, sizeof(srvaddr)) < 0)
    NTHROW("binding socket", 2);

  // Listen for n incoming connections
  if (listen(sckfd, 1) < 0)
    NTHROW("listening for connections", 3);

  printf("Server listening on port %d...\n\x1B[32m", PORT);

  // Wait for an incoming connection
  iplen = sizeof(srvaddr);
  mode[1] = '\0';
  while (1) {
    clifd = accept(sckfd, &cliaddr, &iplen);
    if (clifd < 0)
      NTHROW("accepting connection", 4);

    logg(ClientConnected, true, cliaddr, filename);

    // Read the filename from the client
    n = read(clifd, buffer, BUFFER_SIZE);
    if (n < 0)
      NTHROW("reading filename from the client", 8);
    buffer[n] = '\0'; // remove last dumb char that causes problems
    sprintf(name, "%s", strtok(buffer, ""));
    n = strlen(name);
    if (!filename_ok(name)) {
      char temp[strlen(name) + 40];
      sprintf(temp, "parsing file name (illegal filename)");
      NTHROW(temp, 403);
    }
    mode[0] = name[n - 1];
    if (mode[0] == 'w') {
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
      send(clifd, buffer, strlen(buffer), 0);

    printf("%s sent!\n", filename);

    // Empty the buffer and close the connection
    bzero(buffer, BUFFER_SIZE);
    if (close(clifd) != 0)
      NTHROW("closing file descriptor", 7);
  }

  // Close the file and the client socket
  if (fclose(file) != 0)
    return throw("closing file stream", 6);
  if (close(clifd) != 0)
    return throw("closing file descriptor", 7);
  close(sckfd);

  printf("Streams closed, goodbye!\n");

  return 0;
}
