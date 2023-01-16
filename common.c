// include librairies used in all files
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern int errno;

// preferences
#define PORT 6666
#define BUFFER_SIZE 1024
#define VERBOSE true

// macros
#define print(a, args...)                                                    \
  printf("[%lu]%s(%s:%d) " a, (unsigned long)time(NULL), __func__, __FILE__, \
         __LINE__, ##args)
#define println(a, args...) print(a "\n", ##args)
#define assert                            \
  if (errno != 0)                         \
  {                                       \
    r.code = errno;                       \
    strcpy(r.string, strerror(errno));    \
    println("ER%d-%s", r.code, r.string); \
    return r;                             \
  }
#define assertquit                              \
  if (errno != 0)                               \
  {                                             \
    println("EX%d-%s", errno, strerror(errno)); \
    exit(errno);                                \
  }

struct Result
{
  unsigned int code;
  char string[256];
  FILE *file;
};

/*
 * Log syntax:
 *  - ER = error
 *  - EX = exit (critical error)
 *  - IP = connexion
 *  - RF = read file
 *  - WF = write file
 */