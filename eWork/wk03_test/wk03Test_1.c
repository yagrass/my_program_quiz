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

int main()
{
    signed int iData[] = {-512, -128, -1, -4, -1000, -1000, 1000};
    unsigned int uiShift[] = {3, 4, 1, 1, 1, 2, 2};

    for(int i=0; i < (sizeof(iData)/sizeof(signed int)); i++)
    {
      int iRet = asr_i(iData[i],uiShift[i]);
      printf("###### %d(%08X), shift %d is %d(%08X) ######\n\n", iData[i], iData[i], uiShift[i], iRet, iRet);
    }
}
