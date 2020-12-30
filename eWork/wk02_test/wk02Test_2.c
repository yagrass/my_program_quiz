#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "ansWK02Test.h"
#define MDebug //printf

uint8_t hexchar2val(uint8_t in)
{
    const uint8_t letter = in & MASK;
    const uint8_t shift = (letter >> AAA) | (letter >> BBB);
    return (in + shift) & 0xf;
}

int main(int argc, char const *argv[])
{
  uint8_t cString[] = "0123456789ABCDEFabcdef";

  for(uint8_t *ptr = cString; *ptr; ptr++)
  {
    printf("char %C is %d\n", *ptr, hexchar2val(*ptr));
  }
}
