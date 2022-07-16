#include "stdafx.h"

#include <stdio.h>

int ParseArgs(char **serverAddress, int argc, char **argv) {
    int i;
    for (i = 1; i < argc; i++) {
        if ((argv[i][0] != '-') && (argv[i][0] != '/')) {
            continue;
        }
        switch (argv[i][1]) {
            case 'n':
                if(i+1 < argc) {
                    *serverAddress = argv[++i];
                }
                break;
            default:
                printf("No args parsed\n");
                break;
        }
    }
    return 0;
}
