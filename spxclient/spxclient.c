#include "stdafx.h"

#include <winsock2.h>

#include "conv.h"
#include "args.h"
#include "spxclientloop.h"

#pragma comment(lib, "WS2_32")

char *defaultAddress = "1234CDEF.000000000002";

int main(int argc, char **argv) {
    WSADATA wsd;
    char **serverAddress = &defaultAddress;
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
        printf("WSAStartup() failed with error code %ld\n", GetLastError());
        return -1;
    }
    printf("WSAStartup() is OK!\n");

    ParseArgs(serverAddress, argc, argv);

    printf("Server address: %s\n", *serverAddress);

    ClientMainLoop(*serverAddress);

    if(WSACleanup() == 0) {
        printf("WSACleanup() is OK!\n");
    } else {
        printf("WSACleanup() failed with error code %ld\n", WSAGetLastError());
    }
    printf("Press ENTER to exit...");
    getchar();
    return 0;
}
