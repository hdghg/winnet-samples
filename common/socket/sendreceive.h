#ifndef SENDRECEIVE_H
#define SENDRECEIVE_H
#include "../stdafx.h"

#include <winsock2.h>

int ReceiveData(SOCKET s, char *buffer);
int SendData(SOCKET s, char *pchBuffer, int size);

#endif
