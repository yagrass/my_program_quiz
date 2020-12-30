#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MDebug printf
#define OP ^

int hammingDistance(int x, int y) {
    return __builtin_popcount(x OP y);
}

int main()
{
  uint64_t i64 = 0;
  uint64_t u = 125, v = 5;

  i64 = gcd64n(u, v);
  printf("gcd of %ld and %ld are %ld\n", u, v, i64);

  u = 128; v = 64;
  i64 = gcd64(u, v);
  printf("gcd of %ld and %ld are %ld\n", u, v, i64);
}
