#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MDebug printf
#define OP1 >>1
#define OP2 (m < 0)
int asr_i(signed int m, unsigned int n)
{
    const int logical = (((int) -1) OP1) > 0;
    unsigned int fixu = -(logical & (OP2));
    int fix = *(int *) &fixu;
    MDebug("logical:%d(%X),", logical, logical);
    MDebug(" fixu:(%X), fix:(%X),", fixu,fix);
    MDebug(" m>>n:(%X), fix>>n:(%X),", m>>n, fix>>n);
    MDebug("fix^(fix>>n):(%X)\n", fix ^ (fix>>n));
    return (m >> n) | (fix ^ (fix >> n));
}

unsigned popcnt_branchless(unsigned v)
{
    unsigned n;
    n = (v >> 1) & 0x77777777;
    v -= n;
    MDebug("n=%x, v=%x\n", n, v);
    n = (n >> 1) & 0x77777777;
    v -= n;
    MDebug("n=%x, v=%x\n", n, v);
    n = (n >> 1) & 0x77777777;
    v -= n;
    MDebug("n=%x, v=%x\nv>>4=%x,", n, v, v>>4);
    v = (v + (v >> 4)) & 0x0F0F0F0F;
    MDebug("v&0x0F0F0F0F=%x, ", v);
    v *= 0x01010101;
    MDebug("v*=0x01010101=%x, ret=%d\n", v, v>>24);

    return v >> 24;
}

int main()
{
    unsigned int iData[] = {362, 428, 551, 4414, 12235, 241, 1000};

    // unsigned int n = 100;
    // printf("n=%d(%x), -n=%d(%x)\n", n,n, -n, -n);
    // n = 53;
    // printf("n=%d(%x), -n=%d(%x)\n", n, n, -n, -n);
    // n = -n;
    // printf("n=%d(%x), -n=%d(%x)\n", n, n, -n, -n);

    for(int i=0; i < (sizeof(iData)/sizeof(signed int)); i++)
    {
      unsigned int iRet = popcnt_branchless(iData[i]);
      printf("\t%d(%08X), bit count is %d\n\n", iData[i], iData[i], iRet);
    }
}
