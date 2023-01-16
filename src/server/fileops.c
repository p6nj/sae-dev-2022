#include "../../common.c"
#define NO errno = EACCES

/* Checks if a filename is right for the project format.
 * This will prevent clients from writting somewhere else than the provided
 * folder using either '..' or '/'.
 * On Windows you also have to check for '\'.
 * The filename is considered right only if it's just a name (no extension).
 */
void check_filename(char filename[]) {
  switch (filename[0]) {
  case '.':
    NO;
  case '/':
    NO;
  default:;
  }
  if (strchr(filename, '.') != NULL)
    NO;
  if (strchr(filename, '/') != NULL)
    NO;
  // the access mode must be present at the end and can either be read or write
  switch (filename[strlen(filename) - 1]) {
  case 'w':
    return;
  case 'r':
    return;
  }
  NO;
}

struct Result action(char prompt[]) {
  struct Result r;
  char folder[] = prompt[strlen(prompt) - 1] == 'w' ? "CSV" : "OUT";
  prompt[strlen(prompt) - 1] = '\0';
  return r;
}