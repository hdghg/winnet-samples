#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>
#include <wsnwlink.h>

#include "../conv.h"

// Function: CreateSocket
// Description:
//    creates the main socket (i.e. the listening socket for the
//    server and the connecting socket for the client).
//    SPX sockets use either SOCK_STREAM or SOCK_SEQPACKET but must
//    be of the protocol NSPROTO_SPX or NSPROTO_SPXII.
//    IPX sockets must use SOCK_DGRAM and NSPROTO_IPX.
int CreateSocket(__out SOCKET *sock, __in int type, __in int protocol) {
    *sock = /*WinSock2.*/socket(AF_IPX, type, protocol);
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
void FillIpxAddress(
        __out SOCKADDR_IPX *socketAddress,
        __in char *address,
        __in char *endpoint) {
    char *periodPosition;
    /*WinBase.*/ZeroMemory(socketAddress, sizeof(SOCKADDR_IPX));
    socketAddress->sa_family = AF_IPX;

    // Check if an address is specified
    if (NULL != address) {
        // Get the offset for node number/network number separator
        periodPosition = /*string.*/strchr(address, '.');
        if (NULL == periodPosition) {
            printf("IPX address does not have a separator\n");
            return;
        }
        // convert the address in the  string format to binary format
        AtoH((char *)socketAddress->sa_netnum,  address, 4);
        AtoH((char *)socketAddress->sa_nodenum, periodPosition + 1, 6);
    }
    if (NULL != endpoint) {
        socketAddress->sa_socket = (USHORT)/*stdlib.*/atoi(endpoint);
    }
}

void SockaddrIpxToA(char *dest, SOCKADDR_IPX ipxAddress) {
    unsigned char *netNumber = (unsigned char *) ipxAddress.sa_netnum;
    unsigned char *nodeNumber = (unsigned char *) ipxAddress.sa_nodenum;
    IpxAddressToA(dest, netNumber, nodeNumber);
}

// Function: BindSocket
// Description:
//    BindSocket() binds a socket descriptor 'sock' to
//    the specified address. If an endpoint is specified it uses
//    that or it binds to a system  assigned port.
int BindSocket(
        __in SOCKET *socket,
        __out SOCKADDR_IPX *ipxAddress,
        __in char *address,
        __in char *endpoint) {
    int ipxAddressSize = sizeof(SOCKADDR_IPX);
    char boundAddressStr[22];
    FillIpxAddress(ipxAddress, address, endpoint);

    if (SOCKET_ERROR == /*WinSock2.*/bind(*socket, (SOCKADDR *) ipxAddress, sizeof(SOCKADDR_IPX))) {
        return -1;
    }

    // Print the address we are bound to. If a particular interface is not
    // mentioned in the BindSocket() call, this may print the address as
    // 00000000.0000000000000000. This is because of the fact that an
    // interface is picked only when the actual connection establishment
    // occurs, in case of connection oriented socket
    /*WinSock2.*/getsockname(*socket, (SOCKADDR *)ipxAddress, &ipxAddressSize);
    SockaddrIpxToA(boundAddressStr, *ipxAddress);
    printf("Bound to Local Address: %s\n", boundAddressStr);
    return 0;
}



// Print all available adapters for the socket. Only works with datagram socket
int EnumerateAdapters(SOCKET *sock) {
    SOCKADDR_IPX     sa_ipx;
    IPX_ADDRESS_DATA ipx_data;
    int              ret, cb, nAdapters, i=0;
    char             boundAddressStr[22];

    // Call getsockopt() see the total number of adapters
    cb = sizeof(nAdapters);
    ret = getsockopt(*sock, NSPROTO_IPX, IPX_MAX_ADAPTER_NUM, (char *) &nAdapters, &cb);
    if (ret == SOCKET_ERROR) {
        printf("getsockopt(IPX_MAX_ADAPTER_NUM) failed with error code %ld\n", WSAGetLastError());
        return -1;
    }
    printf("Total number of adapters: %d\n", nAdapters);
    // Get the address of each adapter
    for (i=0; i<nAdapters; i++) {
        memset(&ipx_data, 0, sizeof(ipx_data));
        ipx_data.adapternum = i;
        cb = sizeof(ipx_data);

        ret = getsockopt(*sock, NSPROTO_IPX, IPX_ADDRESS, (char *) &ipx_data, &cb);
        if (ret == SOCKET_ERROR) {
            printf("getsockopt(IPX_ADDRESS) failed with error code %ld\n", WSAGetLastError());
            return -1;
        }

        // Print each address
        IpxAddressToA(boundAddressStr, (unsigned char *) ipx_data.netnum, (unsigned char *) ipx_data.nodenum);
        printf("%d: %s\n", i, boundAddressStr);
        //PrintIpxAddress((char *) ipx_data.netnum, (char *) ipx_data.nodenum);
    }
    return 0;
}

int CloseSocket(SOCKET *socket) {
    if(0 == /*WinSock2.*/closesocket(*socket)) {
        printf("closesocket(sock) is OK!\n");
    } else {
        printf("closesocket(sock) failed with error code %ld\n", WSAGetLastError());
    }
    return 0;
}
