#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MDebug printf

#define QQ0 0x01000000
#define QQ1 0x3f000000
#define QQ2 23

/* A union allowing us to convert between a float and a 32-bit integers.*/
typedef union {
    float value;
    uint32_t v_int;
} ieee_float_shape_type;

/* Set a float from a 32 bit int. */
#define  INSERT_WORDS(d, ix0)	        \
    do {                                \
        ieee_float_shape_type iw_u;     \
            iw_u.v_int = (ix0);         \
        (d) = iw_u.value;               \
    } while (0)

/* Get a 32 bit int from a float. */
#define EXTRACT_WORDS(ix0, d)        \
    do {                             \
        ieee_float_shape_type ew_u;  \
        ew_u.value = (d);            \
        (ix0) = ew_u.v_int;          \
    } while (0)

static const float one = 1.0, tiny = 1.0e-30;

float ieee754_sqrt(float x)
{
    float z;
    int32_t sign = 0x80000000;
    uint32_t r;
    int32_t ix0, s0, q, m, t, i;
    ieee_float_shape_type iZ;
    EXTRACT_WORDS(ix0, x);

    /* take care of zero */
    if (ix0 <= 0) {
        if ((ix0 & (~sign)) == 0)
            return x; /* sqrt(+-0) = +-0 */
        if (ix0 < 0)
            return (x - x) / (x - x); /* sqrt(-ve) = sNaN */
    }
    /* take care of +INF and NaN */
    if ((ix0 & 0x7f800000) == 0x7f800000) {
        /* sqrt(NaN) = NaN, sqrt(+INF) = +INF,*/
        return x;
    }
    /* normalize x */
    m = (ix0 >> 23);
    if (m == 0) { /* subnormal x */
        for (i = 0; (ix0 & 0x00800000) == 0; i++)
            ix0 <<= 1;
        m -= i - 1;
    }
    m -= 127; /* unbias exponent */
    ix0 = (ix0 & 0x007fffff) | 0x00800000;
    if (m & 1) { /* odd m, double x to make it even */
        ix0 += ix0;
    }
    m >>= 1; /* m = [m/2] */

    /* generate sqrt(x) bit by bit */
    ix0 += ix0;
    q = s0 = 0; /* [q] = sqrt(x) */
    r = QQ0;       /* r = moving bit from right to left */
    MDebug("sqrt start (m=%x, ix0=%x)\n", m, ix0);
    while (r != 0) {
        t = s0 + r;
        if (t <= ix0) {
            s0 = t + r;
            ix0 -= t;
            q += r;
        }
        ix0 += ix0;
        r >>= 1;
        MDebug("\tix0:%x,r:%x,t:%x,s0:%x,q:%x\n",
                  ix0, r, t, s0, q);
    }

    iZ.value = one;
    MDebug("one:%f(%x) q:%x\n", iZ.value, iZ.v_int, q);
    iZ.value = tiny;
    MDebug("tiny:%f(%x) q:%x\n", iZ.value, iZ.v_int, q);
    iZ.value = one - tiny;
    MDebug("one-tiny:%f(%x) q:%x\n", iZ.value, iZ.v_int, q);
    /* use floating add to find out rounding direction */
    if (ix0 != 0) {
        z = one - tiny; /* trigger inexact flag */
        if (z >= one) {
            MDebug("trace 1\n");
            z = one + tiny;
            if (z > one)
                q += 2;
            else
                q += (q & 1);
        }
    }
    iZ.value = z;
    MDebug("z:%f(%x) q:%x\n", iZ.value, iZ.v_int, q);

    ix0 = (q >> 1) + QQ1;
    MDebug("ix0 = %x\n", ix0);

    ix0 += (m << QQ2);
    MDebug("ix0 = %x\n", ix0);

    INSERT_WORDS(z, ix0);
    iZ.value = z;
    MDebug("z:%f(%x) q:%x\n", iZ.value, iZ.v_int, q);

    return z;
}

int main(int argc, char *argv[]) {
  ieee_float_shape_type iz;
  int m = 10, i = 2;
  ieee_float_shape_type fRet;
  //
  // iz.value = 0;
  // printf("%f(%e) = %x\n", iz.value, iz.value, iz.v_int);
  //
  iz.value = 1.20;
  printf("%f(%e) = %x\n", iz.value, iz.value, iz.v_int);

  iz.value = 3.140625;
  // int *p = (int *)&iz.value;
  printf("%f(%e) = %x (%x)\n", iz.value, iz.value, iz.v_int, *((unsigned int *)&iz.value));

  // iz.value = 1024.0;
  // printf("%f(%e) = %x\n", iz.value, iz.value, iz.v_int);
  //
  // iz.v_int = 0x3E000000;
  // printf("%f(%e) = %x\n", iz.value, iz.value, iz.v_int);
  //
  // iz.v_int = 0x3E400000;
  // printf("%f(%e) = %x\n", iz.value, iz.value, iz.v_int);
  //
  // iz.v_int = 0x3A000000;
  // printf("%f(%e) = %x\n", iz.value, iz.value, iz.v_int);
  //
  // iz.v_int = 0x3A400000;
  // printf("%f(%e) = %x\n", iz.value, iz.value, iz.v_int);

  iz.v_int = 0x3C000000;
  printf("%f(%e) = %x\n", iz.value, iz.value, iz.v_int);

  iz.v_int = 0x40000000;
  printf("%f(%e) = %x\n", iz.value, iz.value, iz.v_int);

/*
  iz.v_int = 0x00400000;
  iz.value *= 2;
  printf("%f(%e) = %x\n\n", iz.value, iz.value, iz.v_int);

  iz.value = 625.0;
  fRet.value = ieee754_sqrt(iz.value);
  printf("sqrt(%e/%x) = %e/%x\n", iz.value, iz.v_int, fRet.value, fRet.v_int);

  iz.value = 1024.0;
  fRet.value = ieee754_sqrt(iz.value);
  printf("sqrt(%e/%x) = %e/%x\n", iz.value, iz.v_int, fRet.value, fRet.v_int);

  iz.v_int = 0x00400000;
  fRet.value = ieee754_sqrt(iz.value);
  printf("sqrt(%e/%x) = %e/%x\n", iz.value, iz.v_int, fRet.value, fRet.v_int);

  iz.value = 0.16;
  fRet.value = ieee754_sqrt(iz.value);
  printf("sqrt(%e/%x) = %e/%x\n", iz.value, iz.v_int, fRet.value, fRet.v_int);
*/
}
