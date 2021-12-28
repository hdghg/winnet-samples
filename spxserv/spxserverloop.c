#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>

#include "socket/common.h"
#include "socket/sendreceive.h"
#include "conv.h"

void mainLoop(SOCKET *serverSocket) {
    int ipxAddressSize = sizeof(SOCKADDR_IPX);
    SOCKADDR_IPX clientIpxAddress;
    int bytesExchanged;
    char byteBuffer[MAX_DATA_LEN];
    char clientAddressStr[22];
    SOCKET clientSocket = INVALID_SOCKET;

    while (1) {
        clientSocket = /*WinSock2.*/accept(*serverSocket, (SOCKADDR *) &clientIpxAddress, &ipxAddressSize);
        if (clientSocket == INVALID_SOCKET) {
            printf("accept() failed: %d\n", WSAGetLastError());
            return;
        }
        printf("accept() is OK...\n");
        // Print the address of connected client
        SockaddrIpxToA(clientAddressStr, clientIpxAddress);
        printf("Connected to Client Address: %s\n", clientAddressStr);

        while (1) {
            // Receive data on newly created socket
            bytesExchanged = /*sendreceive.*/ReceiveData(clientSocket, byteBuffer);
            if (0 < bytesExchanged) {
                // Print the contents of received data
                byteBuffer[bytesExchanged] = '\0';
                printf("%d bytes of data received: %s\n", bytesExchanged, byteBuffer);
                // Send data on newly created socket
                printf("Sending data...\n");
                bytesExchanged = /*sendreceive.*/SendData(clientSocket, byteBuffer);
            }
            if (bytesExchanged == 0) {
                break;
            } else if (bytesExchanged == -1) {
                CloseSocket(&clientSocket);
                return;
            }
            printf("%d bytes of data sent\n", bytesExchanged);
        }
        printf("Client disconnected: %s\n", clientAddressStr);
        CloseSocket(&clientSocket);
    }
}

int ServerMainLoop() {
    SOCKET serverSocket = INVALID_SOCKET;
    SOCKADDR_IPX serverIpxAddress;
    char *serverEndpointStr = "7171";

    if(0 != CreateSocket(&serverSocket, SOCK_STREAM, NSPROTO_SPX)){
        printf("CreateSocket() failed with error code %ld\n", WSAGetLastError());
        return -1;
    }
    printf("CreateSocket() is OK...\n");
    //setNonBlocking(serverSocket);
    if(0 != BindSocket(&serverSocket, &serverIpxAddress, NULL, serverEndpointStr)) {
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
