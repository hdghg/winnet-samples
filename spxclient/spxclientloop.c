#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>
#include <windows.h>
#include <conio.h>

#include "socket/common.h"
#include "socket/sendreceive.h"
#include "socket/nbsocket.h"
#include "conv.h"

char * nbReadLine(char *dest) {
    int len;
    char c;
    while (_kbhit()) {
        len = strlen(dest);
        c = _getch();
        if ('\t' == c) {
            return NULL;
        }
        if (8 == c && 0 < len) {
            printf("%c %c", c, c);
            dest[len - 1] = '\0';
            return NULL;
        }
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

int mainLoop(SOCKET *socket) {
    int bytesExchanged;
    char receiveBuffer[MAX_DATA_LEN];
    char sendBuffer[MAX_DATA_LEN];
    int operationResult;

    memset(&sendBuffer, '\0', MAX_DATA_LEN);
    while (TRUE) {
        if (nbReadLine(sendBuffer)) {
            memset(&sendBuffer[strlen(sendBuffer)], '\0', MESSAGE_SIZE);
            if (0 != AwaitReadiness(NULL, socket, TRUE)) {
                return -1;
            }
            bytesExchanged = /*sendreceive.*/SendData(*socket, sendBuffer, MESSAGE_SIZE);
            if (bytesExchanged < 1) {
                return 0;
            }
            printf("%d bytes of data sent\n", bytesExchanged);
            memset(&sendBuffer, '\0', MAX_DATA_LEN);
        }
        operationResult = NbCheckReadiness(socket, NULL, FALSE);
        if (0 == operationResult) {
            bytesExchanged = /*sendreceive.*/ReceiveData(*socket, receiveBuffer);
            if (bytesExchanged < 1) {
                if (0 == bytesExchanged) {
                    printf("Server went offline\n");
                }
                return 0;
            }
            receiveBuffer[bytesExchanged] = '\0';
            printf("%d bytes of data received: %s\n", bytesExchanged, receiveBuffer);
        } else if (-3 != operationResult) {
            return -1;
        }
    }
    return 0;
}

int ClientMainLoop(char *serverAddressStr) {
    int ipxAddressSize = sizeof(SOCKADDR_IPX);
    SOCKET clientSocket = INVALID_SOCKET;
    SOCKADDR_IPX localIpxAddress;
    SOCKADDR_IPX serverIpxAddress;
    char *serverEndpointStr = "7171";
    int connectResult;

    if (serverAddressStr == NULL) {
        printf("Server Address must be specified.... Exiting\n");
        return;
    }
    if(0 != CreateSocket(&clientSocket, SOCK_STREAM, NSPROTO_SPX)) {
        printf("CreateSocket() failed with error code %ld\n", WSAGetLastError());
        return;
    }
    printf("CreateSocket() is OK...\n");
    if (0 != SwitchToNonBlocking(&clientSocket)) {
        printf("Couldn't switch socket to non-blocking mode...\n");
        return CloseSocket(&clientSocket);
    }
    if(0 != BindSocket(&clientSocket, &localIpxAddress, NULL, NULL)) {
        printf("BindSocket() failed!\n");
        return CloseSocket(&clientSocket);
    }
    printf("BindSocket() is OK!\n");
    FillIpxAddress(&serverIpxAddress, serverAddressStr, serverEndpointStr);
    printf("Connecting to Server: %s\n", serverAddressStr);
    connectResult = /*WinSock2.*/connect(clientSocket, (SOCKADDR *) &serverIpxAddress, ipxAddressSize);
    if (SOCKET_ERROR == connectResult && WSAEWOULDBLOCK != WSAGetLastError()) {
        printf("Failed to issue a connect request %d\n", WSAGetLastError());
        return CloseSocket(&clientSocket);
    }
    if (0 != AwaitReadiness(NULL, &clientSocket, TRUE)) {
        return CloseSocket(&clientSocket);
    }

    printf("Connected successfully\n");

    mainLoop(&clientSocket);
    return CloseSocket(&clientSocket);
}
