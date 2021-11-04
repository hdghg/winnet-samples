#include "stdafx.h"

#include <winsock2.h>

#include "conv.h"
#include "loop.h"

#pragma comment(lib, "WS2_32")

int main(int argc, char **argv) {
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
        printf("WSAStartup() failed with error code %ld\n", GetLastError());
        return -1;
    }
    printf("WSAStartup() is OK!\n");

    ServerMainLoop();

    if(WSACleanup() == 0) {
        printf("WSACleanup() is OK!\n");
    } else {
        printf("WSACleanup() failed with error code %ld\n",WSAGetLastError());
    }
    printf("Press ENTER to exit...");
    getchar();
    return 0;
}
