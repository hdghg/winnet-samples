#include "stdafx.h"

#include <winsock2.h>

#include <windows.h>

#include "socket/common.h"
#include "socket/sendreceive.h"
#include "socket/nbsocket.h"
#include "io.h"
#include "conv.h"

int mainLoop(SOCKET *socket) {
    int bytesExchanged;
    char receiveBuffer[MESSAGE_SIZE + 1];
    char sendBuffer[MESSAGE_SIZE + 1];
    int operationResult;

    memset(&sendBuffer, '\0', MESSAGE_SIZE + 1);
    while (TRUE) {
        if (/*io.*/NbReadLine(sendBuffer)) {
            memset(&sendBuffer[strlen(sendBuffer)], '\0', MESSAGE_SIZE);
            if (0 != AwaitReadiness(NULL, socket, TRUE)) {
                return -1;
            }
            bytesExchanged = /*sendreceive.*/SendData(*socket, sendBuffer, MESSAGE_SIZE);
            if (bytesExchanged < 1) {
                return 0;
            }
            printf("%d bytes of data sent\n", bytesExchanged);
            memset(&sendBuffer, '\0', MESSAGE_SIZE + 1);
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
    int tcpAddressSize = sizeof(SOCKADDR_IN);
    SOCKET clientSocket = INVALID_SOCKET;
    SOCKADDR_IN serverTcpAddress;
    char *serverEndpointStr = "7171";
    int connectResult;
    {
        serverTcpAddress.sin_family = AF_INET;
        serverTcpAddress.sin_port = htons(7171);
    }

    if (serverAddressStr == NULL) {
        printf("Server Address must be specified.... Exiting\n");
        return 0;
    }
    serverTcpAddress.sin_addr.s_addr = inet_addr(serverAddressStr);

    if(0 != CreateSocket(&clientSocket, AF_INET, SOCK_STREAM, IPPROTO_TCP)) {
        printf("CreateSocket() failed with error code %ld\n", WSAGetLastError());
        return -1;
    }
    printf("CreateSocket() is OK...\n");
    if (0 != SwitchToNonBlocking(&clientSocket)) {
        printf("Couldn't switch socket to non-blocking mode...\n");
        return CloseSocket(&clientSocket);
    }
    printf("Connecting to Server: %s\n", serverAddressStr);
    connectResult = /*WinSock2.*/connect(clientSocket, (SOCKADDR *) &serverTcpAddress, tcpAddressSize);
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
