#include "preferences.c"
#include <stdbool.h>

#define FORMAT "\x1B[31m/!\\ Error %s. /!\\\nThe server will now stop.\n"

int throw(char *desc, int code) {
  if (VERBOSE)
    fprintf(stderr, FORMAT, desc);
  return (code);
}
