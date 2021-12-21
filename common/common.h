#ifndef COMMON_H
#define COMMON_H
#include "stdafx.h"

#include <winsock2.h>

#define MAX_DATA_LEN 64000

int CreateSocket(SOCKET *sock);
void FillIpxAddress(__out SOCKADDR_IPX *socketAddress, __in LPSTR address, __in LPSTR endpoint);
int BindSocket(SOCKET *sock, SOCKADDR_IPX *psa, LPSTR address, LPSTR endpoint);
int EnumerateAdapters(SOCKET *sock);

#endif
