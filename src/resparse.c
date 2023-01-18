// response format: [r/w][filename]|[data]

#include "varshit.c"
#include <stdbool.h>
#include <string.h>

struct request {
    char mode;
    char filename[MAXREQUESTSIZE - 1];
    char filedata[MAXFILESIZE];
};

struct request parse(char response[TOTALMAX]) {
    struct request r;
    r.mode = response[0];
    int i;
    if (r.mode == 'r') {
        for (i = 1; i < MAXREQUESTSIZE; i++) {
            if (response[i] == '|')break;
            sprintf(r.filename + strlen(r.filename), "%c", response[i]);
        }
        for (int j = i; j < strlen(response); j++)
            sprintf(r.filedata, "%c", response[j]);
    }
    else {
        for (i = 1; i < strlen(response); i++)
            sprintf(r.filename, "%c", response[i]);
    }
}

bool response_ok(char response[TOTALMAX]) {
    if (strlen(response) < 2)
        return false;
    // if the response is the right size, the last char should be empty.
    if (response[TOTALMAX] != '\0')
        return false;
    switch (response[0]) {
        case 'w':
            {
                if (strchr(response, "|") == NULL)
                    return false;
                bool data = false;
                int j = 0;
                for (int i = 1; i < TOTALMAX; i++) {
                    if (data) {
                        if (i >= MAXFILESIZE)
                            return false;
                        j += 1;
                    }
                    else if (response[i] == '|') {
                        if (i >= MAXREQUESTSIZE)
                            return false;
                        data = true;
                    }
                }
                if (j < 5)
                    return false;// not enough file data
                return true;
            }
        case 'r':
            return true;
        default:
            return false;
    }
}