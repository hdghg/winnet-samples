#include "stdafx.h"

#include <winsock2.h>
#include <wsipx.h>
#include <wsnwlink.h>

int AwaitReadiness(__in SOCKET *readSocket, __in SOCKET *writeSocket, __in BOOL verbose) {
    int res;
    struct timeval waitTime = {0, 100000};
    struct fd_set readSocketSet;
    struct fd_set writeSocketSet;
    struct fd_set exceptSocketSet;
    while (TRUE) {
        FD_ZERO(&readSocketSet);
        FD_ZERO(&writeSocketSet);
        FD_ZERO(&exceptSocketSet);
        if (NULL != readSocket) {
            FD_SET(*readSocket, &readSocketSet);
            FD_SET(*readSocket, &exceptSocketSet);
        }
        if (NULL != writeSocket) {
            FD_SET(*writeSocket, &writeSocketSet);
            FD_SET(*writeSocket, &exceptSocketSet);
        }
        res = select(0, &readSocketSet, &writeSocketSet, &exceptSocketSet, &waitTime);
        if (SOCKET_ERROR == res) {
            return -2;
        }
        if (NULL != writeSocket && FD_ISSET(*writeSocket, &exceptSocketSet)) {
            return -1;
        }
        if (NULL != readSocket && FD_ISSET(*readSocket, &exceptSocketSet)) {
            return -1;
        }
        if (NULL != writeSocket && FD_ISSET(*writeSocket, &writeSocketSet)) {
            return 0;
        }
        if (NULL != readSocket && FD_ISSET(*readSocket, &readSocketSet)) {
            return 0;
        }
        if (verbose) printf(".");
    }
}

int SwitchToNonBlocking(SOCKET *socket) {
    u_long nonBlockingMode = 1;
    return ioctlsocket(*socket, FIONBIO, &nonBlockingMode);
}
