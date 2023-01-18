// include librairies used in all files (I can't build a deps tree yet)
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include "varshit.c"

// errors
#define FORMAT "\x1B[31m/!\\ Error %s. /!\\\nCODE %u\n"
int throw(char* desc, unsigned int code){
  if (VERBOSE)
    fprintf(stderr, FORMAT, desc, code);
  return code;
}