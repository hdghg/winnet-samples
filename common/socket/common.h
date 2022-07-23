#ifndef COMMON_H
#define COMMON_H
#include "../stdafx.h"

#include <winsock2.h>
#include <wsipx.h>

#define MESSAGE_SIZE 128
#define SAMPLES_MAXCONN 16

int CreateSocket(OUT SOCKET *sock, IN int family, IN int type, IN int protocol);
void FillIpxAddress(OUT SOCKADDR_IPX *socketAddress, IN char *address, IN char *endpoint);
void SockaddrIpxToA(OUT char *dest, IN SOCKADDR_IPX ipxAddress);
int BindSocket(IN SOCKET *sock, OUT SOCKADDR_IPX *psa, IN LPSTR address, IN LPSTR endpoint);
int BindTcpSocket(IN SOCKET *sock, IN SOCKADDR_IN *psa);
int EnumerateAdapters(IN SOCKET *sock);
int CloseSocket(IN SOCKET *socket);
int PrintAddresses();

#endif
