#include "stdafx.h"

#include <winsock2.h>

// ReceiveData() is generic rotuine to receive some data over a
// connection-oriented IPX socket.
int ReceiveData(SOCKET s, char *buffer) {
    int received, totalBytes = 0, leftBytes = 128;
    while (leftBytes > 0) {
        received = /*WinSock2.*/recv(s, &buffer[totalBytes], leftBytes, 0); // blocking operation
        if (SOCKET_ERROR == received) {
            if (WSAEWOULDBLOCK == WSAGetLastError()) {
                printf("recv() failed with error code WSAEWOULDBLOCK\n");
                break;
            }
            if (WSAEDISCON == WSAGetLastError()) {
                printf("Connection closed by peer...\n");
            } else {
                printf("recv() failed with error code %ld\n", WSAGetLastError());
            }
            return -1;
        }
        if (0 == received) {
            printf("recv() is OK...\n");
            break;
        }
        totalBytes += received;
        leftBytes -= received;
    }
    return totalBytes;
}

// SendData() is generic routine to send some data over a
// connection-oriented IPX socket.
int SendData(SOCKET s, char *pchBuffer) {
    int ret;
    ret = /*WinSock2.*/send(s, pchBuffer, strlen(pchBuffer), 0);
    if (SOCKET_ERROR == ret) {
        printf("send() failed with error code %ld\n", WSAGetLastError());
        return -1;
    } else {
        printf("send() is OK...\n");
    }
    return ret;
}