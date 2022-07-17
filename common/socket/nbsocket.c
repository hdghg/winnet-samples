#include "../stdafx.h"

#include <winsock2.h>
#include <wsipx.h>
#include <wsnwlink.h>

#define READY 0;
#define EXCEPTION -2;
#define NOT_READY -3;

int AwaitReadiness(IN SOCKET *readSocket, IN SOCKET *writeSocket, IN BOOL verbose) {
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
            return SOCKET_ERROR;
        }
        if (NULL != writeSocket && FD_ISSET(*writeSocket, &exceptSocketSet)) {
            return EXCEPTION;
        }
        if (NULL != readSocket && FD_ISSET(*readSocket, &exceptSocketSet)) {
            return EXCEPTION;
        }
        if (NULL != writeSocket && FD_ISSET(*writeSocket, &writeSocketSet)) {
            return READY;
        }
        if (NULL != readSocket && FD_ISSET(*readSocket, &readSocketSet)) {
            return READY;
        }
        if (verbose) printf(".");
    }
}

int NbCheckReadiness(IN SOCKET *readSocket, IN SOCKET *writeSocket, IN BOOL verbose) {
    int res;
    struct timeval waitTime = {0, 1000};
    struct fd_set readSocketSet;
    struct fd_set writeSocketSet;
    struct fd_set exceptSocketSet;
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
        return SOCKET_ERROR;
    }
    if (NULL != writeSocket && FD_ISSET(*writeSocket, &exceptSocketSet)) {
        return EXCEPTION;
    }
    if (NULL != readSocket && FD_ISSET(*readSocket, &exceptSocketSet)) {
        return EXCEPTION;
    }
    if (NULL != writeSocket && FD_ISSET(*writeSocket, &writeSocketSet)) {
        return READY;
    }
    if (NULL != readSocket && FD_ISSET(*readSocket, &readSocketSet)) {
        return READY;
    }
    if (verbose) printf(",");
    return NOT_READY;
}

int SwitchToNonBlocking(SOCKET *socket) {
    u_long nonBlockingMode = 1;
    return ioctlsocket(*socket, FIONBIO, &nonBlockingMode);
}
