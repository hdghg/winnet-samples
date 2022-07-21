#include "stdafx.h"

#include <winsock2.h>

#include "socket/common.h"
#include "tcpserverloop.h"

#pragma comment(lib, "WS2_32")

int _tmain(int argc, _TCHAR* argv[]) {
    WSADATA wsd;
    if (0 != WSAStartup(MAKEWORD(2,2), &wsd)) {
        printf("WSAStartup() failed with error code %ld\n", GetLastError());
        return -1;
    }
    printf("WSAStartup() is OK!\n");
    /*common.*/PrintAddresses();

    printf("Starting server...\n");
    /*tcpserverloop.*/ServerMainLoop();

    if(WSACleanup() == 0) {
        printf("WSACleanup() is OK!\n");
    } else {
        printf("WSACleanup() failed with error code %ld\n", WSAGetLastError());
    }
    printf("Press ENTER to exit...");
    getchar();
    return 0;
}
