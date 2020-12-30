#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "ansWK02Test.h"
#define MDebug //printf
bool is_ascii(const char str[], size_t size)
{
    if (size == 0)
        return false;
    int i = 0;
    while ((i + 8) <= size) {
        uint64_t payload;
        memcpy(&payload, str + i, 8);
        MDebug("payload:\t0x%lx\n", payload);
        MDebug("MMM:\t0x%lx\n", MMM);
        if (payload & MMM)
            return false;
        i += 8;
    }
    while (i < size) {
        if (str[i] & 0x80)
            return false;
        i++;
    }
    return true;
}

int main(int argc, char const *argv[])
{
  const char array1[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x78, 0x99, 0xAA, 0x29, 0x46, 0x78, 0x39, 0x97, 0x74, 0xF3, 0x46};
  const char array2[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x78, 0x44, 0x0A, 0x29, 0x46, 0x78, 0x39, 0x57, 0x74, 0x13, 0x46};

  if(is_ascii(array1, sizeof(array1)))
    printf("\narray1 are all ASCII code!\n\n\n");
  else
    printf("\narray1 are not ASCII code!\n\n\n");

  if(is_ascii(array2, sizeof(array2)))
    printf("\narray2 are all ASCII code!\n");
  else
    printf("\narray2 are not ASCII code!\n");
}
