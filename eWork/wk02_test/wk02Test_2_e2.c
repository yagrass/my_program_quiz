#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "ansWK02Test.h"
#define MDebug printf

uint64_t hexchar2val(uint64_t *pIn)
{
    uint8_t len = strlen((uint8_t*)pIn);
    const uint64_t letter = *pIn & MASK64;
    const uint64_t shift = (letter >> AAA) | (letter >> BBB);
    const uint64_t sum = *pIn + shift;
    uint64_t result=0;

    uint8_t *pcPtr = (uint8_t*)&sum;
    for(uint8_t i=0; i<len; i++) {
        result <<= 4;
        result += (uint64_t)(pcPtr[i]&0xf);
        MDebug("\tpcPtr[i]:%X result:%lX\n", pcPtr[i], result);
    }

    printf("str:%s(0x%lX) letter:(%lX) shift:(%lX), sum:(%lX)\n\t> %lX (%ld) \n\t", (uint8_t*)pIn, *pIn, letter, shift, sum, result, result);
    return result;
}

int main(int argc, char const *argv[])
{
  uint8_t pcString[3][11] = {"0xFF", "0xCAFEBABE", "0x8BADF00D"};

  for(int i=0; i<3; i++)
  {
     hexchar2val((uint64_t*)&pcString[i][2]);
  }
}
