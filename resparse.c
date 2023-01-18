#include "varshit.c"
#include <stdbool.h>

struct request {
    char mode;
    char filename[MAXREQUESTSIZE - 1];
    char filedata[MAXFILESIZE];
};

struct request parse(char response[TOTALMAX]) {
    struct request r;
    r.mode = response[0];

}

bool response_ok(char response[TOTALMAX]) {
    if (strlen(response) < 2)return false;
    switch (response[0]) {
        case 'w':
            return true;
        case 'r':
            return true;
        default: return false;
    }
}