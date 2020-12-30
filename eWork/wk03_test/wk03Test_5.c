#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MDebug printf
uint64_t gcd64(uint64_t u, uint64_t v) {
    if (!u || !v) return u | v;
    while (v) {
        uint64_t t = v;
        v = u % v;
        u = t;
    }
    return u;
}

#define XXX v
#define YYY u << shift

uint64_t gcd64n(uint64_t u, uint64_t v) {
    if (!u || !v) return u | v;
    int shift;
    for (shift = 0; !((u | v) & 1); shift++) {
        u /= 2, v /= 2;
    }
    while (!(u & 1))
        u /= 2;
    do {
        while (!(v & 1))
            v /= 2;
        if (u < v) {
            v -= u;
        } else {
            uint64_t t = u - v;
            u = v;
            v = t;
        }
    } while (XXX);
    return YYY;
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
