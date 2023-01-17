#define CLIENTCONNECTED           "CC %s", inet_ntoa(((struct sockaddr_in*) &ip)->sin_addr)
#define CLIENTDISCONNECTED        "CD %s", inet_ntoa(((struct sockaddr_in*) &ip)->sin_addr)
#define READFILE                  "RF %s %s", filename, inet_ntoa(((struct sockaddr_in*) &ip)->sin_addr)
#define WRITEFILE                 "WF %s %s", filename, inet_ntoa(((struct sockaddr_in*) &ip)->sin_addr)
#define SERVERSTARTED             "SS %d", PORT
#define fmt(f) sprintf(format, f);break
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