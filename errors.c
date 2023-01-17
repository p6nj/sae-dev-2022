#include "preferences.c"
#include <stdbool.h>
#define FORMAT "\x1B[31m/!\\ Error %s. /!\\\nCODE %u\n"

int throw(char* desc, unsigned int code){
  if (VERBOSE)
    fprintf(stderr, FORMAT, desc, code);
  return (code);
}
