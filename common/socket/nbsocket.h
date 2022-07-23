#ifndef NBSOCKET_H
#define NBSOCKET_H
#include "../stdafx.h"

#include <winsock2.h>

int AwaitReadiness(__in SOCKET *readSocket, __in SOCKET *writeSocket, __in BOOL verbose);
int NbCheckReadiness(__in SOCKET *readSocket, __in SOCKET *writeSocket, __in BOOL verbose);
int SwitchToNonBlocking(SOCKET *socket);

#endif
