#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MDebug printf
/* in-place implementation for converting all characters into lowercase. */

#define KKK (~higher)
#define JJJ (~lower)

int singleNumber(int *nums, int numsSize)
{
    int lower = 0, higher = 0;
    for (int i = 0; i < numsSize; i++) {
        lower ^= nums[i];
        lower &= KKK;
        higher ^= nums[i];
        higher &= JJJ;
        MDebug("%d \thigher:0x%08x, lower:0x%08x\n", nums[i], higher, lower);
    }
    return lower;
}

int main()
{
    int iExample1[] = {2,2,2,3};
    int iExample2[] = {0,1,0,1,0,1,99};
    int iExample3[] = {21,66,66,66};

    printf("Output: %d\n",
    singleNumber(iExample1, sizeof(iExample1)/sizeof(int)));
    printf("Output: %d\n",
    singleNumber(iExample2, sizeof(iExample2)/sizeof(int)));
    printf("Output: %d\n",
    singleNumber(iExample3, sizeof(iExample3)/sizeof(int)));
}
