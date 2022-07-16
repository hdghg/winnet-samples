#include "stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <wsipx.h>
#include <wsnwlink.h>

#include "socket/common.h"
#include "tcpserverloop.h"

#pragma comment(lib, "WS2_32")

int printAdapters() {
    int tcpAddressSize = sizeof(SOCKADDR_IN);
    SOCKET s = INVALID_SOCKET;
    SOCKADDR_IN tcpAddress;
    printf("Trying to resolve available adapters...\n");
    if(0 != /*common.*/CreateSocket(&s, AF_INET, SOCK_DGRAM, IPPROTO_UDP)){
        return -1;
    }
    tcpAddress.sin_family = AF_INET;
    tcpAddress.sin_addr.s_addr = inet_addr("0.0.0.0");;
    tcpAddress.sin_port = htons(7171);

    if (SOCKET_ERROR == /*WinSock2.*/bind(s, (SOCKADDR *) &tcpAddress, tcpAddressSize)) {
        printf("Could not resolve adapters. ");
        printf("bind() failed with error code %ld\n", WSAGetLastError());
    } else {
        /*WinSock2.*/getsockname(*socket, (SOCKADDR *) &tcpAddress, &tcpAddressSize);
        printf("Bound to %s\n", inet_ntoa(tcpAddress.sin_addr));
        PrintAddresses();
    }

    // Close the bound socket
    if(0 != /*WinSock2.*/closesocket(s)) {
        printf("closesocket(sock) failed with error code %ld\n", WSAGetLastError());
    }
    return 0;
}


int _tmain(int argc, _TCHAR* argv[]) {
    WSADATA wsd;
    if (0 != WSAStartup(MAKEWORD(2,2), &wsd)) {
        printf("WSAStartup() failed with error code %ld\n", GetLastError());
        return -1;
    }
    printf("WSAStartup() is OK!\n");
    printAdapters();

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
