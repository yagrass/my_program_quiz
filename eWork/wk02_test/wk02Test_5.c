#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MDebug printf
/* in-place implementation for converting all characters into lowercase. */

#define PACKED_BYTE(b) (((uint64_t)(b) & (0xff)) * 0x0101010101010101u)
#define VV1 0x80  // 0x10, 0x20, 0x40, 0x60, 0x80
#define VV2 (0)    // (-1), 0, 1
#define VV3 (-1)  // (-1), 0, 1
#define VV4 0x80  // 0x10, 0x20, 0x40, 0x60, 0x80
/* in-place implementation for converting all characters into lowercase. */
void strlower(char *s, size_t n)
{
    for (size_t j = 0; j < n; j++) {
        if (s[j] >= 'A' && s[j] <= 'Z')
            s[j] ^= 1 << 5;
        else if ((unsigned) s[j] >= '\x7f') /* extended ASCII */
            s[j] = tolower(s[j]);
    }
}
/* vectorized implementation for in-place strlower */
void strlower_vector(char *s, size_t n)
{
    size_t k = n / 8;
    for (size_t i = 0; i < k; i++, s += 8) {
        uint64_t *chunk = (uint64_t *) s;
        char acBuff[9] = {0,0,0,0,0,0,0,0,0};
        memcpy((void*)acBuff, (void*)s, 8);
        MDebug("<%s> if(%lX==0)\n",
               acBuff, (*chunk & PACKED_BYTE(VV1)) );

        if ((*chunk & PACKED_BYTE(VV1)) == 0) { /* is ASCII? */
            uint64_t A = *chunk + PACKED_BYTE(128 - 'A' + VV2);
            uint64_t Z = *chunk + PACKED_BYTE(128 - 'Z' + VV3);
            uint64_t mask = ((A ^ Z) & PACKED_BYTE(VV4)) >> 2;
            *chunk ^= mask;
            memcpy((void*)acBuff, (void*)s, 8);
            MDebug("\tA=%lx, Z=%lx, mask=%016lx, *chunk=%lx\n\t=>%s\n\n", A, Z, mask, *chunk, acBuff);
        } else
            strlower(s, 8);
    }

    k = n % 8;
    if (k)
        strlower(s, k);
}
int main()
{
    /* quote from https://www.gutenberg.org/files/74/74-h/74-h.htm */
    char str[] =
        "This eBook is for the use of anyone anywhere at no cost and with \
almost no restrictions whatsoever.  You may copy it, give it away or \
re-use it under the terms of the Project Gutenberg License included \
with this eBook or online at www.gutenberg.net";
    int n = strlen(str);
    strlower_vector(str, n);
    puts(str);
}
