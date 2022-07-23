#include "stdafx.h"
#include <windows.h>

// Function: BtoH
// Description: BtoH () returns the equivalent binary value for an individual
//    character specified in the ascii format.
UCHAR BtoH(char ch) {
    if ((ch >= '0') && (ch <= '9')) {
        return(ch - '0');
    }
    if ((ch >= 'A') && (ch <= 'F')) {
        return(ch - 'A' + 0xA);
    }
    if ((ch >= 'a') && (ch <= 'f')) {
        return(ch - 'a' + 0xA);
    }
    return -1;
}

// Function: AtoH
// Description: AtoH () coverts the IPX address specified in the string
//    (ascii) format to the binary (hexadecimal) format.
void AtoH(char *szDest, char *szSource, int iCount) {
    while (iCount--) {
        *szDest++ = (BtoH(*szSource++) << 4) + BtoH(*szSource++);
    }
}

// Function: IpxAddressToA
// Description: Outputs IPX address to a ascii string 22 characters long
void IpxAddressToA(char *dest, unsigned char *lpsNetnum, unsigned char *lpsNodenum) {
    int i;

    // Print the network number first
    for (i=0; i < 4 ;i++) {
        sprintf_s((char *) dest + 2 * i, 2, "%02X", (UCHAR)lpsNetnum[i]);
    }
    sprintf_s((char *) dest + 8, 2 ,".");

    // Print the node number
    for (i=0; i < 6 ;i++) {
        sprintf_s((char *) dest + 2 * i + 9, 2, "%02X", (UCHAR) lpsNodenum[i]);
    }
}
