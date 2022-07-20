#include "../stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#include <wsipx.h>
#include <wsnwlink.h>

#include "../conv.h"

// Create a socket.\n
int CreateSocket(OUT SOCKET *sock, IN int family, IN int type, IN int protocol) {
    *sock = /*WinSock2.*/socket(family, type, protocol);
    if (INVALID_SOCKET == *sock) {
        if (WSAEAFNOSUPPORT == WSAGetLastError()) {
            printf("Protocol is not supported by the system\n", WSAGetLastError());
        } else {
            printf("CreateSocket() failed with error code %ld\n", WSAGetLastError());
        }
        return -1;
    }
    return 0;
}

// Function: FillIpxAddress
// Description:
//    FillIpxAddress() fills a structure of type SOCKADDR_IPX
//    with relevant address-family, network number, node number
//    and socket (endpoint) parameters
void FillIpxAddress(
        OUT SOCKADDR_IPX *socketAddress,
        IN char *address,
        IN char *endpoint) {
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

void SockaddrIpxToA(OUT char *dest, IN SOCKADDR_IPX ipxAddress) {
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
        IN SOCKET *socket,
        OUT SOCKADDR_IPX *ipxAddress,
        IN char *address,
        IN char *endpoint) {
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

int BindTcpSocket(IN SOCKET *socket, IN SOCKADDR_IN *psa) {
    int tcpAddressSize = sizeof(SOCKADDR_IN);
    if (SOCKET_ERROR == /*WinSock2.*/bind(*socket, (SOCKADDR *) psa, tcpAddressSize)) {
        return -1;
    }
    /*WinSock2.*/getsockname(*socket, (SOCKADDR *) psa, &tcpAddressSize);
    printf("Bound to %s\n", inet_ntoa(psa->sin_addr));
    return 0;
}



// Print all available adapters for the socket. Only works with datagram socket
int EnumerateAdapters(IN SOCKET *sock) {
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

int CloseSocket(IN SOCKET *socket) {
    if(0 == /*WinSock2.*/closesocket(*socket)) {
        printf("closesocket(sock) is OK!\n");
    } else {
        printf("closesocket(sock) failed with error code %ld\n", WSAGetLastError());
    }
    return 0;
}

int PrintAddresses() {
    struct addrinfo hints;
    INT iRetval;
    DWORD dwRetval;
    int i = 1;
    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct sockaddr_in  *sockaddr_ipv4;
    LPSOCKADDR sockaddr_ip;

    char ipstringbuffer[46];
    DWORD ipbufferlength = 46;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
//    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_protocol = AF_UNSPEC;
    printf("Calling getaddrinfo with following parameters:\n");
    printf("\tnodename = %s\n", "");
    printf("\tservname (or port) = %s\n\n", "");

    dwRetval = getaddrinfo("", "0", &hints, &result);
    if (dwRetval != 0) {
        printf("getaddrinfo failed with error: %d\n", dwRetval);
        WSACleanup();
        return 1;
    }

    printf("getaddrinfo returned success\n");
    
    // Retrieve each address and print out the hex bytes
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        printf("getaddrinfo response %d\n", i++);
        printf("\tFlags: 0x%x\n", ptr->ai_flags);
        printf("\tFamily: ");
        switch (ptr->ai_family) {
            case AF_UNSPEC:
                printf("Unspecified\n");
                break;
            case AF_INET:
                printf("AF_INET (IPv4)\n");
                sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
                printf("\tIPv4 address %s\n",
                    inet_ntoa(sockaddr_ipv4->sin_addr) );
                break;
            case AF_INET6:
                printf("AF_INET6 (IPv6)\n");
                // the InetNtop function is available on Windows Vista and later
                // sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
                // printf("\tIPv6 address %s\n",
                //    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );
                
                // We use WSAAddressToString since it is supported on Windows XP and later
                sockaddr_ip = (LPSOCKADDR) ptr->ai_addr;
                // The buffer length is changed by each call to WSAAddresstoString
                // So we need to set it for each iteration through the loop for safety
                ipbufferlength = 46;
                iRetval = WSAAddressToStringA(sockaddr_ip, (DWORD) ptr->ai_addrlen, NULL, 
                    ipstringbuffer, &ipbufferlength );
                if (iRetval)
                    printf("WSAAddressToString failed with %u\n", WSAGetLastError() );
                else    
                    printf("\tIPv6 address %s\n", ipstringbuffer);
                break;
            case AF_NETBIOS:
                printf("AF_NETBIOS (NetBIOS)\n");
                break;
            default:
                printf("Other %ld\n", ptr->ai_family);
                break;
        }
        printf("\tSocket type: ");
        switch (ptr->ai_socktype) {
            case 0:
                printf("Unspecified\n");
                break;
            case SOCK_STREAM:
                printf("SOCK_STREAM (stream)\n");
                break;
            case SOCK_DGRAM:
                printf("SOCK_DGRAM (datagram) \n");
                break;
            case SOCK_RAW:
                printf("SOCK_RAW (raw) \n");
                break;
            case SOCK_RDM:
                printf("SOCK_RDM (reliable message datagram)\n");
                break;
            case SOCK_SEQPACKET:
                printf("SOCK_SEQPACKET (pseudo-stream packet)\n");
                break;
            default:
                printf("Other %ld\n", ptr->ai_socktype);
                break;
        }
        printf("\tProtocol: ");
        switch (ptr->ai_protocol) {
            case 0:
                printf("Unspecified\n");
                break;
            case IPPROTO_TCP:
                printf("IPPROTO_TCP (TCP)\n");
                break;
            case IPPROTO_UDP:
                printf("IPPROTO_UDP (UDP) \n");
                break;
            default:
                printf("Other %ld\n", ptr->ai_protocol);
                break;
        }
        printf("\tLength of this sockaddr: %d\n", ptr->ai_addrlen);
        printf("\tCanonical name: %s\n", ptr->ai_canonname);
    }

    freeaddrinfo(result);

    return 0;

}