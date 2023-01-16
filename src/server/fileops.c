#include "../../common.c"
#include <unistd.h>
#define NO errno = EACCES

/* Checks if a filename is right for the project format.
 * This will prevent clients from writting somewhere else than the provided
 * folder using either '..' or '/'.
 * On Windows you also have to check for '\'.
 * The filename is considered right only if it's just a name (no extension).
 */
void check_filename(char filename[])
{
  switch (filename[0])
  {
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
  switch (filename[strlen(filename) - 1])
  {
  case 'w':
    return;
  case 'r':
    return;
  }
  NO;
}

struct Result getdata(int clientfd)
{
  struct Result r;
  char prompt[BUFFER_SIZE], buffer[BUFFER_SIZE];
  int n = read(clientfd, buffer, BUFFER_SIZE);
  assert;
  buffer[n] = '\0'; // remove last dumb char that causes problems
  sprintf(prompt, "%s", strtok(buffer, ""));
  r.code = 0;
  strcpy(r.string, prompt);
  return r;
}

struct Result getfile(char prompt[], int clientfd)
{
  struct Result r;
  char mode[2];
  mode[0] = prompt[strlen(prompt) - 1];
  mode[1] = '\0';
  char folder[4];
  strcpy(folder, mode[0] == 'w' ? "OUT" : "CSV");
  char filename[strlen(prompt) + 7];
  char buffer[BUFFER_SIZE];
  prompt[strlen(prompt) - 1] = '\0';
  sprintf(filename, "%s/%s.csv", folder, prompt);
  if (mode[0] == 'w')
    println("WF-%s", filename);
  else
    println("RF-%s", filename);
  // Open the CSV file
  FILE *file = fopen(filename, mode);
  assert;
  if (mode[0] == 'w')
  {
    r = getdata(clientfd);
    assert;
    fprintf(file, "%s", r.string);
  }
  r.file = file;
  return r;
}