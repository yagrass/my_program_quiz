#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MDebug printf
#define ZZZ 1-i

int consecutiveNumbersSum(int N)
{
    if (N < 1)
        return 0;
    int ret = 1;
    int x = N;
    for (int i = 2; i < x; i++) {
        x += ZZZ;
        if (x % i == 0)
            ret++;
        MDebug("\tret:%d, x=%d, i=%d\n", ret, x, i);    
    }
    return ret;
}

int main(int argc, char *argv[]) {
    int aiData[] = {5, 9, 15, 23, 127, 425};

    for( int i = 0; i < sizeof(aiData)/sizeof(int); i++) {
      int iRet = consecutiveNumbersSum(aiData[i]);
      printf("input data:%d, output is %d\n", aiData[i], iRet);
    }
}
