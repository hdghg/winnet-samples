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
    if(0 != /*common.*/CreateSocket(&s, SOCK_DGRAM, NSPROTO_IPX)){
        printf("CreateSocket() failed with error code %ld\n", WSAGetLastError());
        return;
    }
    printf("CreateSocket() is OK...\n");
    // Bind to a local address and endpoint
    if(0 == BindSocket(&s, &ipxSockaddr, NULL, NULL)) {
        printf("BindSocket() is OK!\n");
    } else {
        printf("BindSocket() failed!\n");
    }
    if (0 != EnumerateAdapters(&s)) {
        printf("Could not enumerate adapters\n");
    }
    // Close the bound socket
    if(0 == /*WinSock2.*/closesocket(s)) {
        printf("closesocket(sock) is OK!\n");
    } else {
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
