// Structure of every connection: one request, one response.
// File name for edited CSV documents: [original file name]-[year]-[month]-[day]-[hour]-[duration]

#include "src/log.c"
#include "src/fileshit.c"
#include "src/netshit.c"
#include "src/resparse.c"

#define SUCCESS true
#define EPIC_SUCCESS SUCCESS
#define FAILURE false
#define EPIC_FAIL FAILURE
#define logsuccess logg(event, true, cliaddr, filename, __LINE__)
#define logfailure logg(event, false, cliaddr, filename, __LINE__)
#define NTHROW(desc, code)    \
  {                           \
    logfailure;               \
    close(sckfd);             \
    return throw(desc, code); \
  }
#define THROW(desc, code) {logfailure;return throw(desc, code);}

void printerrno() {
  printf("errno=%d, strerr=%s\n", errno, strerror(errno));
}

int main() {
  int sckfd, clifd, n, ncount;
  unsigned int iplen, event;
  struct sockaddr_in srvaddr, cliresaddr;
  struct sockaddr cliaddr;
  FILE* file;
  char buffer[BUFFER_SIZE], filename[MAXREQUESTSIZE];
  char Filename[MAXREQUESTSIZE + 8];
  char mode[2];          // access mode (read or write) for the destination file
  char folder[] = "CSV"; // name of the folder to save or read file
  unsigned int counter = 0;
  char response[TOTALMAX];// one will be used to see if the response is too large
  struct request r;

  atexit(printerrno);

  event = ServerStarted;
  // Create the server socket
  if ((sckfd = suckfd()) < 0)
    THROW("creating socket", 1);

  // Bind the socket to a local address
  // !!!!!!!!!! IF YOU GET AN ERROR HERE CHECK YOUR PORT NUMBER !!!!!!!!!!!!!!
  if (!cast(sckfd, srvaddr, PORT))
    NTHROW("binding socket", 2);

  // Listen for n incoming connections
  if (listen(sckfd, 1) < 0)
    NTHROW("listening for connections", 3);
  printf("Server listening on port %d...\n\x1B[32m", PORT);
  logsuccess;

  // Wait for an incoming connection
  iplen = sizeof(srvaddr);
  mode[1] = '\0';
  while (1) {
    bzero(response, TOTALMAX);
    close(sckfd);
    event = ServerStarted;
    // Create the server socket
    if ((sckfd = suckfd()) < 0)
      THROW("creating socket", 1);

    // Bind the socket to a local address
    // !!!!!!!!!! IF YOU GET AN ERROR HERE CHECK YOUR PORT NUMBER !!!!!!!!!!!!!!
    if (!cast(sckfd, srvaddr, PORT))
      NTHROW("binding socket", 2);

    // Listen for n incoming connections
    if (listen(sckfd, 1) < 0)
      NTHROW("listening for connections", 3);

    clifd = accept(sckfd, &cliaddr, &iplen);
    cliresaddr.sin_family = AF_INET;
    cliresaddr.sin_addr.s_addr = inet_addr(inet_ntoa(((struct sockaddr_in*) &cliaddr)->sin_addr));
    cliresaddr.sin_port = htons(RESPORT);
    event = ClientConnected;
    if (clifd < 0)
      NTHROW("accepting connection", 4);
    logsuccess;

    // Read the filename from the client
    event = ReadFile;
    ncount = 0;
    while ((n = read(clifd, buffer, BUFFER_SIZE)) > 0) {
      if ((ncount + n) > TOTALMAX)
        NTHROW("receiving request: request size exceeded TOTALMAX", 666);
      buffer[n] = '\0';
      sprintf(response + strlen(response), "%s", strtok(buffer, " "));
      printf("buffer = %s\nresponse = %s\n", buffer, response);
      ncount += n;
    }
    if (close(clifd) != 0)
      NTHROW("closing file descriptor", 7);

    // Empty the buffer and close the connection
    bzero(buffer, BUFFER_SIZE);
    if (n < 0)
      NTHROW("reading filename from the client", 8);

    // parse the resonse after verifying it
    if (!response_ok(response)) {
      char temp[strlen(response) + 40];
      sprintf(temp, "parsing response (illegal response)\nresponse = %s\nstrlen(response) = %lu", response, strlen(response));
      NTHROW(temp, 402);
    }
    r = parse(response);
    strcpy(filename, r.filename);
    mode[0] = r.mode;

    if (!filename_ok(filename)) {
      char temp[strlen(filename) + 40];
      sprintf(temp, "parsing file name (illegal filename)\nfilename = %s", filename);
      NTHROW(temp, 403);
    }
    if (mode[0] == 'w') {
      folder[0] = 'O';
      event = WriteFile;
    }
    sprintf(Filename, "%s/%s.csv", folder, filename);

    // Open the CSV file
    file = fopen(Filename, mode);
    if (file == NULL)
      NTHROW("opening file", 5);

    if (mode[0] == 'w') {
      fprintf(file, "%s\n", r.filedata);
      fclose(file);
      logsuccess;
      event = ReadFile;
      file = fopen(Filename, "r");
      if (file == NULL)
        NTHROW("opening file again", 50);
    }

    // reconnect to the client
    close(sckfd);
    // Create the server socket
    if ((sckfd = suckfd()) < 0)
      THROW("creating socket", 1);
    if (connect(sckfd, (struct sockaddr*) &cliresaddr, sizeof(cliresaddr)) < 0)
      return throw("connecting back to client", 404);

    // Read the file and send its contents to the client
    while (fgets(buffer, BUFFER_SIZE, file) != NULL)
      send(sckfd, buffer, strlen(buffer), 0);

    printf("%s sent!\n", filename);
    logsuccess;

    // Empty the buffer and close the connection
    bzero(buffer, BUFFER_SIZE);
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

// snippet to read a large file (tested in client.c):
// // Réception et affichage du contenu du fichier
// char response[BUFFER_SIZE * 10];
// while ((n = read(sockfd, buf, BUFFER_SIZE)) > 0) {
//   buf[n] = '\0';
//   sprintf(response + strlen(response), "%s", buf);
// }
// if (n < 0)
//   return throw("reading server response", 100);
// printf("%s", response);