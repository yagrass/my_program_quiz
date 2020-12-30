#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#define MDebug printf
const uint32_t D = 3;
#define M ((uint64_t)(UINT64_C(0xFFFFFFFFFFFFFFFF) / (D) +1 ))
#define MM ((uint64_t)(UINT64_C(0x1000000) / (D) ))
#define NN ((uint64_t)(UINT64_C(0xFFFFFFFFFFFFFFFF) / (D) ))
/* compute (n mod d) given precomputed M */

void printUI128(__uint128_t n)
{
  uint64_t m = (uint64_t)(n>> 64);
  // printf("%" PRIx64"-",m);
  printf("%016lx-",m);
  m = (n & (__uint128_t)0xFFFFFFFFFFFFFFFF);
  // printf("%" PRIx64,m);
  printf("%016lx",m);
}

uint32_t quickmod(uint32_t n)
{
    uint64_t quotient = ((__uint128_t)M * n) >> 64;
    uint64_t quotientNN = ((__uint128_t)NN * n) >> 64;

    MDebug("\tM= %lx, M*n= ", M);
    printUI128((__uint128_t)M*n);
    MDebug(", q=%lx\n",quotient);

    MDebug("\tNN=%lx, NN*n=", NN);
    printUI128((__uint128_t)NN*n);
    MDebug(", q=%lx\n",quotientNN);

    return n - quotient * D;
}

int main(int argc, char const *argv[])
{
  uint32_t aData[8] = {5, 55, 555, 12, 9, 100, 4, 3};
  uint32_t result;
  __uint128_t testValue = ((__uint128_t)0x1234567890ABCDEF<<8);
  printUI128(testValue);printf("\n");
  for(int i=0; i<8; i++) {
      result = quickmod(aData[i]);
      printf("mode of %d is: %d \n\n", aData[i], result);
  }
}
