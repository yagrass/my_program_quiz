#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
// #include "ansWK02Test.h"  #define XXX cannot work here!
#define MDebug printf
const uint32_t D = 3;
#define M ((uint64_t)(UINT64_C(0xFFFFFFFFFFFFFFFF) / (D) +1 ))
// #define YYY (M + 1)
#define YYYY M
#define YYY (M-1)
// #define YYY (M >> 1)
// #define YYY (M << 1)
/* compute (n mod d) given precomputed M */
uint32_t quickmod(uint32_t n)
{   uint64_t quotient = ((__uint128_t) M * n) >> 8;
    return n - quotient * D;
}

_Bool divisible(uint32_t n)
{
    printf(" n*M=%lx M=%lx, YYY=%lx", n*M, M, YYY);
    return n * M <= YYY;
}

int main(int argc, char const *argv[])
{
  uint32_t aData[8] = {5, 55, 555, 12, 9, 100, 4, 3};
  uint32_t result;
  for(int i=0; i<8; i++) {
      // result = quickmod(aData[i]);
      _Bool bRet = divisible(aData[i]);
      printf("\n=> %d divisible by 3 is %s\n\n", aData[i], bRet? "True": "False");
  }
}
