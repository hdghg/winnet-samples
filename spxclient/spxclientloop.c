#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>
#include <windows.h>
#include <conio.h>

#include "common.h"
#include "conv.h"
#include "sendreceive.h"
#include "socket/nbsocket.h"


char * nbReadLine(char *dest) {
    int len;
    char c;
    if (_kbhit()) {
        len = strlen(dest);
        c = _getch();
        if (('\r' == c) || ('\n' == c)) {
            printf("\n");
            return dest;
        }
        if (127 < len) {
        } else {
            dest[len] = c;
            printf("%c", c);
        }
    }
    return NULL;
}

int mainLoop(SOCKET *socket, char *serverAddressStr) {
    int ipxAddressSize = sizeof(SOCKADDR_IPX);
    SOCKADDR_IPX localIpxAddress;
    SOCKADDR_IPX serverIpxAddress;
    char *localAddressStr = NULL;
    char *serverEndpointStr = "7171";
    int bytesExchanged;
    char receiveBuffer[MAX_DATA_LEN];
    char sendBuffer[MAX_DATA_LEN];
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

    memset(&sendBuffer, '\0', 256);
    while (TRUE) {
        if (nbReadLine(sendBuffer)) {
            /*string.*///memset(byteBuffer, ' ', 128);
            memset(&sendBuffer[strlen(sendBuffer)], ' ', 256);
            sendBuffer[128] = '\0';

            if (0 != AwaitReadiness(NULL, socket, TRUE)) {
                return -1;
            }
            bytesExchanged = /*sendreceive.*/SendData(*socket, sendBuffer);
            if (bytesExchanged < 1) {
                return 0;
            }
            printf("%d bytes of data sent\n", bytesExchanged);
            memset(&sendBuffer, '\0', 256);
        }
        operationResult = NbCheckReadiness(socket, NULL, FALSE);
        if (0 == operationResult) {
            // Receive data from the server
            bytesExchanged = /*sendreceive.*/ReceiveData(*socket, receiveBuffer);
            if (bytesExchanged < 1) {
                return 0;
            }
            // Print the contents of received data
            receiveBuffer[bytesExchanged] = '\0';
            printf("%d bytes of data received: %s\n", bytesExchanged, receiveBuffer);
        } else {
            if (-3 == operationResult) {
                //pass
            } else {
                return -1;
            }
        }
    }
    return 0;
}

void ClientMainLoop(char *serverAddress) {
    SOCKET clientSocket = INVALID_SOCKET;
    printf("Client main loop\n");

    mainLoop(&clientSocket, serverAddress);
    /*WinSock2.*/closesocket(clientSocket);
}
