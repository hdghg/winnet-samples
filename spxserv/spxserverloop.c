#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>

#include "common.h"
#include "conv.h"
#include "sendreceive.h"

void setNonBlocking(SOCKET *socket) {
    u_long nonBlockingMode = 1;
    ioctlsocket(*socket, FIONBIO, &nonBlockingMode);
}

void mainLoop(SOCKET *serverSocket) {
    int ipxAddressSize = sizeof(SOCKADDR_IPX);
    SOCKADDR_IPX serverIpxAddress;
    SOCKADDR_IPX clientIpxAddress;
    char *serverAddressStr = NULL;
    char *serverEndpointStr = "7171";
    int bytesExchanged;
    char byteBuffer[MAX_DATA_LEN];
    char clientAddressStr[22];
    SOCKET clientSocket = INVALID_SOCKET;

    if(0 != CreateSocket(serverSocket, SOCK_STREAM, NSPROTO_SPX)){
        printf("CreateSocket() failed with error code %ld\n", WSAGetLastError());
        return;
    }
    printf("CreateSocket() is OK...\n");
    //setNonBlocking(serverSocket);
    if(0 != BindSocket(serverSocket, &serverIpxAddress, serverAddressStr, serverEndpointStr)) {
        printf("BindSocket() failed!\n");
        if (WSAEADDRINUSE == WSAGetLastError()) {
            printf("Address already in use!\n");
        }
        return;
    }
    printf("BindSocket() is OK!\n");

    if (SOCKET_ERROR == /*WinSock2.*/listen(*serverSocket, SOMAXCONN)) {
        printf("listen() failed with error code %ld\n", WSAGetLastError());
        return;
    }
    printf("listen() looks fine! Waiting for a Connection...\n");

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
                return;
            }
            printf("%d bytes of data sent\n", bytesExchanged);
        }
        printf("Client disconnected: %s\n", clientAddressStr);
        /*WinSock2.*/closesocket(clientSocket);
    }
}

void ServerMainLoop() {
    SOCKET serverSocket = INVALID_SOCKET;

    mainLoop(&serverSocket);

    // Close the bound socket
    if(0 == /*WinSock2.*/closesocket(serverSocket)) {
        printf("closesocket(sock) is OK!\n");
    } else {
        printf("closesocket(sock) failed with error code %ld\n", WSAGetLastError());
    }
}
