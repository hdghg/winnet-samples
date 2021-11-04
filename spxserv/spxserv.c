// spxserv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "conv.h"


int main(int argc, char **argv) {
    int i;
    char dest[9];
    char dest2[22];
    unsigned char addr[4] = {1, 11, 97, 164};
    unsigned char addr2[6] = {1, 11, 97, 164, 2, 2};
    for (i=0; i < 4 ;i++) {
        sprintf((char *) &dest + 2 * i, "%02X", addr[i]);
    }
    printf("%s\n", dest);
    IpxAddressToA((char *) &dest2, addr, addr2);
    printf("%s\n", dest2);

    printf("\nFour: %d\n", getFour());
    getchar();
    return 0;
}
