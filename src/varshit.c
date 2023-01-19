// preferences
#define PORT 6666
#define RESPORT 9999
#define BUFFER_SIZE 1024
#define VERBOSE true
#define MAXFILESIZE BUFFER_SIZE*10
#define MAXREQUESTSIZE BUFFER_SIZE // includes the first letter (r/w)
#define TOTALMAX MAXFILESIZE+MAXREQUESTSIZE // includes the last '\0' character