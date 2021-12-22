#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>
#include <wsnwlink.h>

#include "common.h"
#include "spxserverloop.h"

#pragma comment(lib, "WS2_32")

int printAdapters() {
    SOCKET s = INVALID_SOCKET;
    SOCKADDR_IPX ipxSockaddr;
    printf("Trying to resolve available adapters...\n");
    if(0 != /*common.*/CreateSocket(&s, SOCK_DGRAM, NSPROTO_IPX)){
        printf("Could not resolve adapters. ");
        printf("CreateSocket() failed with error code %ld\n", WSAGetLastError());
        return;
    }
    if(0 != /*common.*/BindSocket(&s, &ipxSockaddr, NULL, NULL)) {
        printf("Could not resolve adapters. ");
        printf("BindSocket() failed with error code %ld\n", WSAGetLastError());
    } else {
        if (0 != EnumerateAdapters(&s)) {
            printf("Could not enumerate adapters\n");
        }
    }
    // Close the bound socket
    if(0 != /*WinSock2.*/closesocket(s)) {
        printf("closesocket(sock) failed with error code %ld\n", WSAGetLastError());
    }
    return 0;
}

int main(int argc, char **argv) {
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
        printf("WSAStartup() failed with error code %ld\n", GetLastError());
        return -1;
    }
    printf("WSAStartup() is OK!\n");
    printAdapters();

    printf("Starting server...\n");
    /*spxserverloop.*/ServerMainLoop();

    if(WSACleanup() == 0) {
        printf("WSACleanup() is OK!\n");
    } else {
        printf("WSACleanup() failed with error code %ld\n", WSAGetLastError());
    }
    printf("Press ENTER to exit...");
    getchar();
    return 0;
}
