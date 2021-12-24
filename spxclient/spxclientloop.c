#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>
#include <windows.h>

#include "common.h"
#include "conv.h"
#include "sendreceive.h"

int setNonBlocking(SOCKET *socket) {
    u_long nonBlockingMode = 1;
    return ioctlsocket(*socket, FIONBIO, &nonBlockingMode);
}

int waitWriteReadiness(SOCKET *socket) {
    int res;
    struct timeval waitTime = {0, 100000};
    struct fd_set write_s;
    struct fd_set except_s;
    while (TRUE) {
        FD_ZERO(&write_s);
        FD_ZERO(&except_s);
        FD_SET(*socket, &write_s);
        FD_SET(*socket, &except_s);
        res = select(0, NULL, &write_s, &except_s, &waitTime);
        if (SOCKET_ERROR == res) {
            printf("\n!!!Failed to await non-blocking socket readiness %d\n", WSAGetLastError());
            return -1;
        }
        if (FD_ISSET(*socket, &except_s)) {
            printf("\nConnection attempt failed\n");
            return -2;
        }
        if (FD_ISSET(*socket, &write_s)) {
            printf("\nNon-blocking connection successful!\n");
            return 0;
        }
        printf(".");
    }
}


int mainLoop(SOCKET *socket, char *serverAddress) {
    SOCKADDR_IPX ipx;
    SOCKADDR_IPX ipxServer;
    char outputBuffer[MAX_DATA_LEN];
    int ret;
    DWORD i;
    char *localAddress = NULL;
    char *serverEndpoint = "7171";

    if(0 != CreateSocket(socket, SOCK_STREAM, NSPROTO_SPX)) {
        printf("CreateSocket() failed with error code %ld\n", WSAGetLastError());
        return -1;
    }
    printf("CreateSocket() is OK...\n");
    if (0 != setNonBlocking(socket)) {
        printf("Couldn't switch socket to non-blocking mode...\n");
        return -1;
    }
    printf("Socket switched to non-blocking mode...\n");

    // Bind to a local address and endpoint
    if(0 != BindSocket(socket, &ipx, localAddress, NULL)) {
        printf("BindSocket() failed!\n");
        return -1;
    }
    printf("BindSocket() is OK!\n");

    // Fill the sa_ipx_server address with server address and endpoint
    if (serverAddress == NULL) {
        printf("Server Address must be specified.... Exiting\n");
        return -1;
    }
    FillIpxAddress(&ipxServer, serverAddress, serverEndpoint);

    printf("Connecting to Server: %s\n", serverAddress);

    // Connect to the server
    ret = /*WinSock2.*/connect(*socket, (SOCKADDR *) &ipxServer, sizeof(ipxServer));
    if (SOCKET_ERROR == ret && WSAEWOULDBLOCK != WSAGetLastError()) {
        printf("Failed to issue a connect request %d\n", WSAGetLastError());
        return -1;
    }

    if (0 != waitWriteReadiness(socket)) {
        return -1;
    }

    printf("Connected to Server Address: %s\n", serverAddress);

    // Send data to the specified server
    /*string.*/memset(outputBuffer, '$', 128);
    outputBuffer[128] = 0;

    for (i=0; i < 5 ;i++) {
        ret = /*sendreceive.*/SendData(*socket, outputBuffer);
        /*winbase.*/Sleep(500);
        if (ret < 1) {
            return 0;
        }
        printf("%d bytes of data sent\n", ret);

        // Receive data from the server
        ret = /*sendreceive.*/ReceiveData(*socket, outputBuffer);
        if (ret < 1) {
            return 0;
        }
        // Print the contents of received data
        outputBuffer[ret] = '\0';
        printf("%d bytes of data received: %s\n", ret, outputBuffer);
    }
    /*winbase.*/Sleep(5000);
    return 0;
}

void ClientMainLoop(char *serverAddress) {
    SOCKET clientSocket = INVALID_SOCKET;
    printf("Client main loop\n");

    mainLoop(&clientSocket, serverAddress);
    /*WinSock2.*/closesocket(clientSocket);
}
