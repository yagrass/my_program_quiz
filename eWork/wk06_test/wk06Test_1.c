#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MDebug  printf

#define BB1 0xFF800000
#define BB2 0xFFFF0000

float fp32tobf16(float x) {
    float y = x;
    int *py = (int *) &y;
    unsigned int exp, man;
    exp = *py & 0x7F800000u;
    man = *py & 0x007FFFFFu;
    if (!exp && !man) /* zero */
        return x;
    if (exp == 0x7F800000u) /* infinity or NaN */
        return x;

    /* Normalized number. round to nearest */
    float r = x;
    int *pr = (int *) &r;
    MDebug("\t*pr=%x, r=%x(%f)\n", *pr, *((int *)&r), r);
    *pr &= BB1;
    MDebug("\t*pr=%x, r=%x(%f)\n", *pr, *((int *)&r), r);
    r /= 256;
    MDebug("\t*pr=%x, r=%x(%f)\n", *pr, *((int *)&r), r);
    y = x + r;
    MDebug("\t*y=%f(%x), x=%f(%x), r=%f(%x)\n", y, *((int *)&y), x, *((int *)&x), r, *((int *)&r));

    *py &= BB2;
    MDebug("\t=====> y=%f(%x)\n", y, *((int *)&y));
    return y;
}

void print_hex(float x) {
    int *p = (int *) &x;
    printf("%f=%x\n", x, *p);
}

int main() {
    float a[] = {3.140625, 1.2, 2.31, 3.46, 5.63};
    for (int i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
        print_hex(a[i]);
        float bf_a = fp32tobf16(a[i]);
        print_hex(bf_a);
        printf("\n");
    }

    return 0;
}
