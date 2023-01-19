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
    if (r.mode == 'w') {
        for (i = 1; i < MAXREQUESTSIZE; i++) {
            if (response[i] == '|')break;
            sprintf(r.filename + strlen(r.filename), "%c", response[i]);
        }
        i++;
        for (int j = i; j < strlen(response); j++)
            sprintf(r.filedata + strlen(r.filedata), "%c", response[j]);
    }
    else {
        for (i = 1; i < strlen(response); i++)
            sprintf(r.filename + strlen(r.filename), "%c", response[i]);
    }
    return r;
}

bool response_ok(char response[TOTALMAX]) {
    if (strlen(response) < 2)
        return false;
    switch (response[0]) {
        case 'w':
            {
                if (strchr(response, '|') == NULL)
                    return false;
                bool data = false;
                int j = 0;
                for (int i = 1; i < TOTALMAX; i++) {
                    if (data) {
                        if (response[i] == '\0')break;
                        else if (j == MAXFILESIZE)
                            return false;
                        j++;
                    }
                    else if (response[i] == '|') {
                        if (i == MAXREQUESTSIZE || i < 3)
                            return false;
                        data = true;
                    }
                }
                if (j < 5)
                    return false;// not enough file data
                return true;
            }
        case 'r':
            return true; // there may be some other checks to do here, feels weird
        default:
            return false;
    }
}
