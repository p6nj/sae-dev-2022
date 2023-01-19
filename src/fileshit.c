#include<stdbool.h>
#include<stdio.h>

/* Checks if a filename is right for the project format.
 * This will prevent clients from writting somewhere else than the provided
 * folder using either '..' or '/'.
 * On Windows you also have to check for '\'.
 * The filename is considered right only if it's just a name (no extension).
 */
bool filename_ok(char filename[]) {
    switch (filename[0]) {
        case '.':
            return false;
        case '/':
            return false;
    }
    if (strchr(filename, '.') != NULL)
        return false;
    if (strchr(filename, '/') != NULL)
        return false;
    return true;
}