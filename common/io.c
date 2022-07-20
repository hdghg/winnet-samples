#include "stdafx.h"
//#include <windows.h>
#include <conio.h>

#include "socket/common.h"

// Read a line in non-blocking way.
// Expected to be run repeatedly until result is not NULL
// Returns NULL when line is not completed
// Returns pointer to dest argument after user hits ENTER
char* NbReadLine(char *dest) {
    size_t len;
    char c;
    while (_kbhit()) {
        len = strlen(dest);
        c = _getch();
        // Tab is ignored
        if ('\t' == c) {
            return NULL;
        }
        // Backspace
        if (8 == c && 0 < len) {
            printf("%c %c", c, c);
            dest[len - 1] = '\0';
            return NULL;
        }
        if (('\r' == c) || ('\n' == c)) {
            printf("\n");
            return dest;
        }
        if (len < MESSAGE_SIZE) {
            dest[len] = c;
            printf("%c", c);
        }
    }
    return NULL;
}
