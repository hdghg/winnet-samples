#include "../stdafx.h"

#include <winsock2.h>

#include "common.h"

// When calling recv in blocking mode, the operation would block
// until socket is closed on the other side, or own buffer is full.
// If socket has been closed on the other side, recv can be called
// many times without any error, each time 0 would be returned
int ReceiveData(SOCKET s, char *buffer) {
    int received, totalBytes = 0, leftBytes = MESSAGE_SIZE;
    while (leftBytes > 0) {
        received = /*WinSock2.*/recv(s, &buffer[totalBytes], leftBytes, 0);
        if (SOCKET_ERROR == received) {
            if (WSAEWOULDBLOCK == WSAGetLastError()) {
                break;
            }
            if (WSAECONNRESET == WSAGetLastError()) {
                printf("An existing connection was forcibly closed by the remote host.\n");
            } else {
                printf("recv() failed with error code %ld\n", WSAGetLastError());
            }
            return -1;
        }
        if (0 == received) {
            break;
        }
        totalBytes += received;
        leftBytes -= received;
    }
    return totalBytes;
}

int SendData(SOCKET socket, char *pchBuffer, int size) {
    int sent;
    sent = /*WinSock2.*/send(socket, pchBuffer, size, 0);
    if (SOCKET_ERROR == sent) {
        printf("send() failed with error code %ld\n", WSAGetLastError());
        return -1;
    }
    printf("send() is OK...\n");
    return sent;
}
