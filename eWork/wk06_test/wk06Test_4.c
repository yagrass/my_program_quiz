#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define MDebug  printf
#define CCC c1 < c2

int findDuplicate(int *nums, int numsSize)
{
    int res = 0;
    const size_t log_2 = 8 * sizeof(int) - __builtin_clz(numsSize);
    MDebug("\tlog_2=%ld,sizeof(int):%ld, __builtin_clz(numsSize)=%d numsSize:%d\n", log_2, sizeof(int), __builtin_clz(numsSize), numsSize);
    for (size_t i = 0; i < log_2; i++) {
        int bit = 1 << i;
        int c1 = 0, c2 = 0;
        MDebug("\t\ti=%ld, bit=%x, res=%d\n", i, bit,res);
        for (size_t k = 0; k < numsSize; k++) {
            MDebug("\t\t\tk=%ld,nums[k]=%d,", k, nums[k]);
            if (k & bit)
                ++c1;
            if (nums[k] & bit)
                ++c2;
            MDebug("c1=%d,c2=%d\n", c1, c2);
        }
        if (CCC)
            res += bit;
    }
    MDebug("\t ------> res=%d", res);
    return res;
}

void printList(int *pList, int size)
{
  printf("list:");
  for (int i = 0; i < size; i++)
    printf("%d, ", pList[i]);
  printf("\n");
}

int main()
{
    int aiList_I[] = {1,3,2,2,2};
    int aiList_II[] = {3,1,3,4,2};
    int aiList_III[] = {1,1};
    int aiList_IV[] = {1,1,2};
    int *pList, size;


    pList = aiList_I; size = sizeof(aiList_I)/sizeof(int);
    printf("input:");
    printList(pList, size);
    printf("output:%d\n\n", findDuplicate(pList, size));

    pList = aiList_II; size = sizeof(aiList_II)/sizeof(int);
    printf("input:");
    printList(pList, size);
    printf("output:%d\n\n", findDuplicate(pList, size));

    pList = aiList_III; size = sizeof(aiList_III)/sizeof(int);
    printf("input:");
    printList(pList, size);
    printf("output:%d\n\n", findDuplicate(pList, size));

    pList = aiList_IV; size = sizeof(aiList_IV)/sizeof(int);
    printf("input:");
    printList(pList, size);
    printf("output:%d\n\n", findDuplicate(pList, size));
}
