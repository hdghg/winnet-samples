// common.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>
#include <wsnwlink.h>

#include "conv.h"

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
    printf("getsockopt(IPX_MAX_ADAPTER_NUM) is OK. Total number of adapters: %d\n", nAdapters);
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
        printf("getsockopt(IPX_ADDRESS) #%d is OK...\n", i);

        // Print each address
        IpxAddressToA(boundAddressStr, (unsigned char *) ipx_data.netnum, (unsigned char *) ipx_data.nodenum);
        printf("%d: %s\n", i, boundAddressStr);
        //PrintIpxAddress((char *) ipx_data.netnum, (char *) ipx_data.nodenum);
    }
}
