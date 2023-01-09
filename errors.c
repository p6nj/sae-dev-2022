#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define VERBOSE true
#define FORMAT "\x1B[31m/!\\ Error %s. /!\\\nThe server will now stop.\n"

/* Binds minimal error descriptions to specific error codes. */
char *desc(int code) {
  switch (code) {
  case 1:
    return "creating socket";
  case 2:
    return "binding socket";
  case 3:
    return "listening for connections";
  case 4:
    return "accepting connection";
  case 5:
    return "opening file";
  case 6:
    return "closing file stream";
  case 7:
    return "closing file descriptor";
  case 8:
    return "reading filename from the client";
  default:
    return "(no description available)";
  }
}

int throw(int code) {
  if (VERBOSE)
    fprintf(stderr, FORMAT, desc(code));
  return(code);
}
