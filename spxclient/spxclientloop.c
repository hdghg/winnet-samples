#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>
#include <windows.h>

#include "common.h"
#include "conv.h"
#include "sendreceive.h"
#include "socket/nbsocket.h"

int mainLoop(SOCKET *socket, char *serverAddressStr) {
    int ipxAddressSize = sizeof(SOCKADDR_IPX);
    SOCKADDR_IPX localIpxAddress;
    SOCKADDR_IPX serverIpxAddress;
    char *localAddressStr = NULL;
    char *serverEndpointStr = "7171";
    int bytesExchanged;
    char byteBuffer[MAX_DATA_LEN];
    int operationResult;
    int counter;

    if (serverAddressStr == NULL) {
        printf("Server Address must be specified.... Exiting\n");
        return -1;
    }

    if(0 != CreateSocket(socket, SOCK_STREAM, NSPROTO_SPX)) {
        printf("CreateSocket() failed with error code %ld\n", WSAGetLastError());
        return -1;
    }
    printf("CreateSocket() is OK...\n");
    if (0 != SwitchToNonBlocking(socket)) {
        printf("Couldn't switch socket to non-blocking mode...\n");
        return -1;
    }
    printf("Socket switched to non-blocking mode...\n");

    // Bind to a local address and endpoint
    if(0 != BindSocket(socket, &localIpxAddress, localAddressStr, NULL)) {
        printf("BindSocket() failed!\n");
        return -1;
    }
    printf("BindSocket() is OK!\n");

    // Fill the sa_ipx_server address with server address and endpoint
    FillIpxAddress(&serverIpxAddress, serverAddressStr, serverEndpointStr);

    printf("Connecting to Server: %s\n", serverAddressStr);

    // Connect to the server
    operationResult = /*WinSock2.*/connect(*socket, (SOCKADDR *) &serverIpxAddress, ipxAddressSize);
    if (SOCKET_ERROR == operationResult && WSAEWOULDBLOCK != WSAGetLastError()) {
        printf("Failed to issue a connect request %d\n", WSAGetLastError());
        return -1;
    }

    if (0 != AwaitReadiness(NULL, socket, TRUE)) {
        return -1;
    }

    printf("Connected successfully\n");

    // Send data to the specified server
    /*string.*/memset(byteBuffer, '$', 128);
    byteBuffer[128] = '\0';

    for (counter = 0; counter < 5; counter++) {
        if (0 != AwaitReadiness(NULL, socket, FALSE)) {
            return -1;
        }
        bytesExchanged = /*sendreceive.*/SendData(*socket, byteBuffer);
        /*winbase.*/Sleep(500);
        if (bytesExchanged < 1) {
            return 0;
        }
        printf("%d bytes of data sent\n", bytesExchanged);

        // Receive data from the server
        bytesExchanged = /*sendreceive.*/ReceiveData(*socket, byteBuffer);
        if (bytesExchanged < 1) {
            return 0;
        }
        // Print the contents of received data
        byteBuffer[bytesExchanged] = '\0';
        printf("%d bytes of data received: %s\n", bytesExchanged, byteBuffer);
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
