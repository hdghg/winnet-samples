#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>

#include "common.h"
#include "conv.h"
#include "sendreceive.h"

void mainLoop(SOCKET *serverSocket) {
    SOCKADDR_IPX sa_ipx;  // Server address
    SOCKADDR_IPX peerAddress; // Client address
    char chBuffer[MAX_DATA_LEN]; // Data buffer
    char peerAddressStr[22];
    int ret,cb;
    char *pszLocalAddress = NULL;                           // Local IPX address string
    char *pszServerEndpoint = "7171";                         // Server's endpoint (socket) string
    SOCKET newsock;

    if(0 != CreateSocket(serverSocket, SOCK_STREAM, NSPROTO_SPX)){
        printf("CreateSocket() failed with error code %ld\n", WSAGetLastError());
        return;
    }
    printf("CreateSocket() is OK...\n");
    // Bind to a local address and endpoint
    if(0 == BindSocket(serverSocket, &sa_ipx, pszLocalAddress, pszServerEndpoint)) {
        printf("BindSocket() is OK!\n");
    } else {
        printf("BindSocket() failed!\n");
    }
    ret = /*WinSock2.*/listen(*serverSocket, SOMAXCONN);
    if (ret == SOCKET_ERROR) {
        printf("listen() failed with error code %ld\n", WSAGetLastError());
    } else {
        printf("listen() looks fine!\n");
        printf("Waiting for a Connection...\n");
    }

    while (1) {
        cb = sizeof(peerAddress);
        newsock = /*WinSock2.*/accept(*serverSocket, (SOCKADDR *) &peerAddress, &cb);
        if (newsock == INVALID_SOCKET) {
            printf("accept() failed: %d\n", WSAGetLastError());
            return;
        } else {
             printf("accept() is OK...\n");
        }
        // Print the address of connected client
        printf("Connected to Client Address: ");
        IpxAddressToA(peerAddressStr, (unsigned char *)peerAddress.sa_netnum, (unsigned char *)peerAddress.sa_nodenum);
        printf("%s\n", peerAddressStr);

        while (1) {
            // Receive data on newly created socket
            ret = /*sendreceive.*/ReceiveData(newsock, chBuffer);
            if (0 < ret) {
                // Print the contents of received data
                chBuffer[ret] = '\0';
                printf("%d bytes of data received: %s\n", ret, chBuffer);
                // Send data on newly created socket
                printf("Sending data...\n");
                ret = /*sendreceive.*/SendData(newsock, chBuffer);
            }
            if (ret == 0) {
                break;
            } else if (ret == -1) {
                return;
            }
            printf("%d bytes of data sent\n", ret);
        }
        /*WinSock2.*/closesocket(newsock);
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
