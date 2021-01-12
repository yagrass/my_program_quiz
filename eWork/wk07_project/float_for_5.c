#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
typedef float data_t;
typedef struct{
    long length;
    data_t *data;
} vec_rec, *vec_ptr;
#define ASM_SUPPORT
void for_5(long length, data_t *src_v1, data_t *src_v2, data_t *dest, vec_ptr u, vec_ptr v)
{
    data_t sum = 0;
    // data_t chk0=0, chk1=0, chk2=0, chk3=0;

#ifndef ASM_SUPPORT      
    for (long i = 0; i < length;  i++ ) 
    {          
        sum = sum + src_v1[i] * src_v2[i];
    }
#else        
    long i = 0;
for_loop:
    asm volatile ("vmovss 0(%2,%1,8), %%xmm1\n\t"              
            "vmulss (%3,%1,8), %%xmm1, %%xmm1\n\t"
            "movl %0, %%eax\n\t"
            "cvtsi2ss %%eax, %%xmm0\n\t"
            "vaddss %%xmm1, %%xmm0, %%xmm0\n\t"
            "cvtss2si %%xmm0, %%eax\n\t"
            "movl %%eax, %0\n\t"
            "addq $1, %1\n\t"
        : "+r" (sum), "+r" (i) //, "=r" (chk0), "=r" (chk1), "=r" (chk2), "=r" (chk3)
        : [src_v1] "r" (src_v1), [src_v2] "r" (src_v2)
        : "eax", "xmm0", "xmm1"
    );

    asm goto ("cmpq %0, %1\n\t"
            "jne %l2\n\t"
        : // no output
        : [length] "r" (length), [i] "r" (i)
        : "cc"
        : for_loop    
    );
#endif            
    printf("%f,\n", sum);
    // printf("\t(%f,%f,%f,%f)\n",chk0,chk1,chk2,chk3);    

    *dest = sum;
// .L15:
//     vmovsd 0(%rbp,%rcx,8), %xmm1
//     vmulsd (%rax,%rcx,8), %xmm1, %xmm1
//     vaddsd %xmm1, %xmm0, %xmm0
//     addq $1, %rcx
//     cmpq %rbx, %rcx
//     jne .L15

}

int main()
{
    data_t v1[] = {10, 20, 30};
    data_t v2[] = {200, 300, 400};
    data_t output;
    for_5(3, (data_t *)v1, (data_t *)v2, (data_t *)&output, (vec_ptr)0, (vec_ptr)0);

    printf("output: %f\n", output);
}