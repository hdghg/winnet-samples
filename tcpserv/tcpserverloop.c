#include "stdafx.h"

#include <winsock2.h>

#include "socket/common.h"
#include "socket/sendreceive.h"
#include "socket/nbsocket.h"
#include "conv.h"

void mainLoop(SOCKET *serverSocket) {
    int ipxAddressSize = sizeof(SOCKADDR_IPX);
    SOCKADDR_IPX clientIpxAddress;
    int bytesExchanged;
    char byteBuffer[MAX_DATA_LEN];
    char clientAddressStr[22];
    SOCKET clientSockets[16];
    SOCKET socket;
    int clientsCount = 16;
    int counter;
    int operationResult;

    while (0 < clientsCount) {
        clientsCount--;
        clientSockets[clientsCount] = INVALID_SOCKET;
    }

    while (TRUE) {
        operationResult = -1;
        for (counter = 0; counter < clientsCount; counter++) {
            operationResult = NbCheckReadiness(&clientSockets[counter], NULL, FALSE);
            if (0 == operationResult) {
                bytesExchanged = /*sendreceive.*/ReceiveData(clientSockets[counter], byteBuffer);
                if (bytesExchanged < 1) {
                    if (0 == bytesExchanged) {
                        printf("Client went offline\n");
                    } else {
                        printf("Connection terminated\n");
                    }
                    socket = clientSockets[counter];
                    clientSockets[counter] = clientSockets[clientsCount - 1];
                    clientSockets[clientsCount - 1] = socket;
                    CloseSocket(&socket);
                    clientsCount--;
                    operationResult = -1;
                    break;
                }
                printf("%d bytes of data received: %s\n", bytesExchanged, byteBuffer);
                break;
            } else if (-3 != operationResult) {
                return;
            }
        }
        if (0 == operationResult) {
            for (counter = 0; counter < clientsCount; counter++) {
                bytesExchanged = /*sendreceive.*/SendData(clientSockets[counter], byteBuffer, MESSAGE_SIZE);
                if (1 > bytesExchanged) {
                    socket = clientSockets[counter];
                    clientSockets[counter] = clientSockets[clientsCount - 1];
                    clientSockets[clientsCount - 1] = socket;
                    CloseSocket(&socket);
                    clientsCount--;
                }
            }
        }

        if (16 <= clientsCount) {
            continue;
        }
        clientSockets[clientsCount] =
            /*WinSock2.*/accept(*serverSocket, (SOCKADDR *) &clientIpxAddress, &ipxAddressSize);
        if (INVALID_SOCKET == clientSockets[clientsCount]) {
            if (WSAEWOULDBLOCK != WSAGetLastError()) {
                printf("accept() failed: %d\n", WSAGetLastError());
                return;
            }
        } else {
            clientsCount++;
            SockaddrIpxToA(clientAddressStr, clientIpxAddress);
            printf("Client %s connected\n", clientAddressStr);
        }
    }
}

int ServerMainLoop() {
    SOCKET serverSocket = INVALID_SOCKET;
    SOCKADDR_IN serverIpAddress;

    serverIpAddress.sin_family = AF_INET;
    serverIpAddress.sin_addr.s_addr = inet_addr("0.0.0.0");
    serverIpAddress.sin_port = htons(7171);

    if(0 != CreateSocket(&serverSocket, AF_INET, SOCK_STREAM, IPPROTO_TCP)){
        return -1;
    }
    printf("CreateSocket() is OK...\n");
    if (0 != /*nbsocket.*/SwitchToNonBlocking(&serverSocket)) {
        printf("Couldn't switch socket to non-blocking mode...\n");
        return CloseSocket(&serverSocket);
    }

    if(0 != BindTcpSocket(&serverSocket, &serverIpAddress)) {
        printf("BindSocket() failed!\n");
        if (WSAEADDRINUSE == WSAGetLastError()) {
            printf("Address already in use!\n");
        }
        return CloseSocket(&serverSocket);
    }
    printf("BindSocket() is OK!\n");

    if (SOCKET_ERROR == /*WinSock2.*/listen(serverSocket, SOMAXCONN)) {
        printf("listen() failed with error code %ld\n", WSAGetLastError());
        return CloseSocket(&serverSocket);
    }
    printf("listen() looks fine! Waiting for a Connection...\n");

    mainLoop(&serverSocket);
    return CloseSocket(&serverSocket);
}
