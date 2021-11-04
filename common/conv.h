#ifndef CONV_H
#define CONV_H
#include "stdafx.h"

UCHAR BtoH(char ch);
void AtoH(char *szDest, char *szSource, int iCount);
void IpxAddressToA(char *dest, unsigned char *lpsNetnum, unsigned char *lpsNodenum);

#endif
