#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>

#include "common.h"
#include "conv.h"

// Function: CreateSocket
// Description:
//    Create a socket based upon the command line parameters. This
//    creates the main socket (i.e. the listening socket for the
//    server and the connecting socket for the client).
//    SPX sockets use either SOCK_STREAM or SOCK_SEQPACKET but must
//    be of the protocol NSPROTO_SPX or NSPROTO_SPXII.
//    IPX sockets must use SOCK_DGRAM and NSPROTO_IPX.
int createSocket(SOCKET *sock) {
    *sock = /*WinSock2.*/socket(AF_IPX, SOCK_STREAM, NSPROTO_SPX);
    if (INVALID_SOCKET == *sock) {
        printf("socket() failed with error code %ld\n", WSAGetLastError());
        return -1;
    }
    printf("socket() looks fine!\n");
    return 0;
}

// Function: FillIpxAddress
// Description:
//    FillIpxAddress() fills a structure of type SOCKADDR_IPX
//    with relevant address-family, network number, node number
//    and socket (endpoint) parameters
void fillIpxAddress(SOCKADDR_IPX *psa, LPSTR lpsAddress, LPSTR lpsEndpoint) {
    LPSTR pszPoint;
    /*WinBase.*/ZeroMemory(psa, sizeof(SOCKADDR_IPX));
    psa->sa_family = AF_IPX;

    // Check if an address is specified
    if (NULL != lpsAddress) {
        // Get the offset for node number/network number separator
        pszPoint = /*string.*/strchr(lpsAddress, '.');
        if (NULL == pszPoint) {
            printf("IPX address does not have a separator\n");
            return;
        }
        // convert the address in the  string format to binary format
        AtoH((char *)psa->sa_netnum,  lpsAddress, 4);
        AtoH((char *)psa->sa_nodenum, pszPoint + 1, 6);
    }
    if (NULL != lpsEndpoint) {
        psa->sa_socket = (USHORT)/*stdlib.*/atoi(lpsEndpoint);
    }
}

// Function: BindSocket
// Description:
//    BindSocket() binds the global socket descriptor 'sock' to
//    the specified address. If an endpoint is specified it uses
//    that or it binds to a system  assigned port.
int bindSocket(SOCKET *sock, SOCKADDR_IPX *psa, LPSTR lpsAddress, LPSTR lpsEndpoint) {
    int ret, len;
    char boundAddressStr[22];
    // Fill the givenSOCKADDR_IPX structure
    fillIpxAddress(psa, lpsAddress, lpsEndpoint);

    ret = /*WinSock2.*/bind(*sock, (SOCKADDR *) psa, sizeof (SOCKADDR_IPX));
    if (SOCKET_ERROR == ret) {
        printf("bind() failed with error code %ld\n", WSAGetLastError());
        return -1;
    }
    printf("bind() is OK...\n");

    // Print the address we are bound to. If a particular interface is not
    // mentioned in the BindSocket() call, this may print the address as
    // 00000000.0000000000000000. This is because of the fact that an
    // interface is picked only when the actual connection establishment
    // occurs, in case of connection oriented socket
    len = sizeof(SOCKADDR_IPX);
    getsockname(*sock, (SOCKADDR *)psa, &len);
    IpxAddressToA(boundAddressStr, (unsigned char *) psa->sa_netnum, (unsigned char *) psa->sa_nodenum);
    printf("Bound to Local Address: %s\n", boundAddressStr);
    return 0;
}

// ReceiveData() is generic rotuine to receive some data over a
// connection-oriented IPX socket.
int receiveData(SOCKET s, char *buffer) {
    int received, totalBytes = 0, leftBytes = 128;
    while (leftBytes > 0) {
        received = /*WinSock2.*/recv(s, &buffer[totalBytes], leftBytes, 0); // blocking operation
        if (SOCKET_ERROR == received) {
            if (WSAEWOULDBLOCK == WSAGetLastError()) {
                printf("recv() failed with error code WSAEWOULDBLOCK\n");
                break;
            }
            if (WSAEDISCON == WSAGetLastError()) {
                printf("Connection closed by peer...\n");
            } else {
                printf("recv() failed with error code %ld\n", WSAGetLastError());
            }
            return -1;
        }
        if (0 == received) {
            printf("recv() is OK...\n");
            break;
        }
        totalBytes += received;
        leftBytes -= received;
    }
    return totalBytes;
}

// SendData() is generic routine to send some data over a
// connection-oriented IPX socket.
int sendData(SOCKET s, char *pchBuffer) {
    int ret;
    ret = /*WinSock2.*/send(s, pchBuffer, strlen(pchBuffer), 0);
    if (SOCKET_ERROR == ret) {
        printf("send() failed with error code %ld\n", WSAGetLastError());
        return -1;
    } else {
        printf("send() is OK...\n");
    }
    return ret;
}

void mainLoop(SOCKET *serverSocket) {
    SOCKADDR_IPX sa_ipx;  // Server address
    SOCKADDR_IPX peerAddress; // Client address
    char chBuffer[MAX_DATA_LEN]; // Data buffer
    char peerAddressStr[22];
    int ret,cb;
    char *pszLocalAddress = NULL;                           // Local IPX address string
    char *pszServerEndpoint = "7171";                         // Server's endpoint (socket) string
    SOCKET newsock;


    if(0 != createSocket(serverSocket)){
        printf("CreateSocket() failed with error code %ld\n", WSAGetLastError());
        return;
    }
    printf("CreateSocket() is OK...\n");
    // Bind to a local address and endpoint
    if(0 == bindSocket(serverSocket, &sa_ipx, pszLocalAddress, pszServerEndpoint)) {
        printf("BindSocket() is OK!\n");
    } else {
        printf("BindSocket() failed!\n");
    }
    if (0 != EnumerateAdapters(serverSocket)) {
        printf("Could not enumerate adapters\n");
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
            ret = receiveData(newsock, chBuffer);
            if (0 < ret) {
                // Print the contents of received data
                chBuffer[ret] = '\0';
                printf("%d bytes of data received: %s\n", ret, chBuffer);
                // Send data on newly created socket
                printf("Sending data...\n");
                ret = sendData(newsock, chBuffer);
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
