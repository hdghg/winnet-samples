#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>


void ServerMainLoop() {
    SOCKET sock = INVALID_SOCKET;

    printf("Server stub\n");

    // Close the bound socket
    if(closesocket(sock) == 0) {
        printf("closesocket(sock) is OK!\n");
    } else {
        printf("closesocket(sock) failed with error code %ld\n", WSAGetLastError());
    }
}