#include "stdafx.h"

#include <winsock2.h>

#include "socket/common.h"
#include "socket/sendreceive.h"
#include "socket/nbsocket.h"
#include "conv.h"

void closeOneSocket(SOCKET clientSockets[], int index, int size) {
    SOCKET socket;
    socket = clientSockets[index];
    clientSockets[index] = clientSockets[size - 1];
    clientSockets[size - 1] = socket;
    CloseSocket(&socket);
}

int tryAccept(IN SOCKET *serverSocket, IN OUT SOCKET clientSockets[], IN OUT int *size) {
    SOCKADDR_IN socketAddress;
    int addressSize = sizeof(SOCKADDR_IN);
    clientSockets[*size] =
        /*WinSock2.*/accept(*serverSocket, (SOCKADDR *) &socketAddress, &addressSize);
    if (INVALID_SOCKET == clientSockets[*size]) {
        if (WSAEWOULDBLOCK != WSAGetLastError()) {
            printf("accept() failed: %d\n", WSAGetLastError());
            return -1;
        }
    } else {
        (*size)++;
        printf("Client %s connected\n", inet_ntoa(socketAddress.sin_addr));
    }
    return 0;
}

void mainLoop(SOCKET *serverSocket) {
    int bytesExchanged;
    char byteBuffer[MESSAGE_SIZE + 1];
    SOCKET clientSockets[SAMPLES_MAXCONN];
    int clientsCount = SAMPLES_MAXCONN;
    int counter;
    int operationResult;
    struct timeval waitTime = {0, 1000};
    struct fd_set readSocketSet;
    {
        FD_ZERO(&readSocketSet);
        FD_SET(*serverSocket, &readSocketSet);
        byteBuffer[MESSAGE_SIZE] = '\0';
        while (0 < clientsCount) {
            clientsCount--;
            clientSockets[clientsCount] = INVALID_SOCKET;
        }
    }

    while (TRUE) {
        operationResult = -1;
        for (counter = 0; counter < clientsCount; counter++) {
            operationResult = NbCheckReadiness(&clientSockets[counter], NULL, FALSE);
            if (-3 == operationResult) {
                continue;
            }
            if (0 != operationResult) {
                return;
            }
            bytesExchanged = /*sendreceive.*/ReceiveData(clientSockets[counter], byteBuffer);
            if (bytesExchanged < 1) {
                printf(0 == bytesExchanged ? "Client went offline\n" : "Client terminated\n");
                closeOneSocket(clientSockets, counter, clientsCount--);
                operationResult = -1;
                break;
            }
            printf("%d bytes of data received: %s\n", bytesExchanged, byteBuffer);
            break;
        }
        // If read successfully from any socket
        if (0 == operationResult) {
            for (counter = 0; counter < clientsCount; counter++) {
                bytesExchanged = /*sendreceive.*/SendData(clientSockets[counter], byteBuffer, MESSAGE_SIZE);
                if (bytesExchanged < 1) {
                    closeOneSocket(clientSockets, counter, clientsCount--);
                }
            }
        }

        if (SAMPLES_MAXCONN <= clientsCount) {
            continue;
        }
        if (select(0, &readSocketSet, NULL, NULL, &waitTime) > 0) {
            if (-1 == tryAccept(serverSocket, clientSockets, &clientsCount)) {
                return;
            }
        } else {
            FD_ZERO(&readSocketSet);
            FD_SET(*serverSocket, &readSocketSet);
        }
    }
}

// Initialize server and pass control to the inner server loop
int ServerMainLoop() {
    SOCKET serverSocket = INVALID_SOCKET;
    int tcpAddressSize = sizeof(SOCKADDR_IN);
    SOCKADDR_IN serverIpAddress;
    {
        serverIpAddress.sin_family = AF_INET;
        serverIpAddress.sin_addr.s_addr = inet_addr("0.0.0.0");
        serverIpAddress.sin_port = htons(7171);
    }

    if(0 != CreateSocket(&serverSocket, AF_INET, SOCK_STREAM, IPPROTO_TCP)){
        printf("Couldn't create socket\n");
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
        } else {
            printf("Bind error: %ld!\n", WSAGetLastError());
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
