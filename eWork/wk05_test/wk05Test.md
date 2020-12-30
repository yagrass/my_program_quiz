# 2020q3 Homework5 (quiz5)
contributed by < ``yceugene`` >

## 1. divop:

### divop():
```c=
#include <stdio.h>
#include <stdlib.h>

double divop(double orig, int slots) {
    if (slots == 1 || orig == 0)
        return orig;
    int od = slots & 1;
    double result = divop(orig / D1, od ? (slots + D2) >> 1 : slots >> 1);
    if (od)
        result += divop(result, slots);
    return result;
}
```
* 動作說明 :
    * 一開始先決定遞迴函式終止條件,先檢查被除數是否為 '0', 及除數是否為'1',若成立,即返回被除數.
    * 接著由於判斷除數為奇術還是偶數:
        * 偶數則直接計算 $\dfrac{X}{Y}$ $=$ $\dfrac{X/2}{Y/2}$
        * 奇數則依 $\dfrac{X}{Y}$ $=$ $\dfrac{X}{Y+1}$ $+$ $\dfrac{X}{Y(Y+1)}$ 拆成兩部分,分別計算,加總並傳回結果.
    * 在此, 選擇 D1 = 2, D2 = 1 以符合結果.

### ieee754_sqrt():
```c=
#include <stdint.h>

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

    while (r != 0) {
        t = s0 + r;
        if (t <= ix0) {
            s0 = t + r;
            ix0 -= t;
            q += r;
        }
        ix0 += ix0;
        r >>= 1;
    }

    /* use floating add to find out rounding direction */
    if (ix0 != 0) {
        z = one - tiny; /* trigger inexact flag */
        if (z >= one) {
            z = one + tiny;
            if (z > one)
                q += 2;
            else
                q += (q & 1);
        }
    }

    ix0 = (q >> 1) + QQ1;
    ix0 += (m << QQ2);

    INSERT_WORDS(z, ix0);

    return z;
}
```

![](https://i.imgur.com/w7OB2ne.png)

![](https://i.imgur.com/Qic4wPO.png)

:::spoiler  IEEE754 32bits floating point detail:
floating point (32bits): smmm mmmm mfff - ffff ffff ffff ffff
* 1.0000  -> 0x3f800000 ->  0011 1111  1000 0000 - 0000 0000  0000 0000
>   m=01111111 f=0 exp = m-127 = 0, $2^{0}$ = 1

* 3.0000  -> 0x40400000 ->  0100 0000  0100 0000 - 0000 0000  0000 0000
>   m=10000000 f=0x400000 exp = m-127 = 1, $2^{1}$ = 2
>   f=0x400000 = $\dfrac{2}{2}$ = 1
>   sum = 2 + 1 = 3

* 4.0000  -> 0x40800000 ->  0100 0000  8000 0000 - 0000 0000  0000 0000
>   m=10000001 f=0 exp = m-127 = 2, $2^{2}$ = 4

* 1024.00 -> 0x44800000 ->  0100 0100  1000 0000 - 0000 0000  0000 0000
> m=10001001 f=0 exp = m-127 = 10, $2^{10}$ = 1024

* 25.000 ->  0x41c8-0000 -> 0100 0001  1100 1000 - 0000 0000
> m=10000011 exp = m-127 = 4 $2^{4}$ = 16
> f=10010000 = 9,
> 16 + 9 = 25

* 0.12500 -> 0x3e000000 ->  0011 1110  0000 0000 - 0000 0000  0000 0000
> m=01111100 f=0 exp = m-127 = -3, $2^{-3}$ = $\dfrac{1}{8}$ = 0.125  

* 0.18750 -> 0x3e400000 ->  0011 1110  0100 0000 - 0000 0000  0000 0000
>  m=01111100 f=100 0000 - 0000 0000  0000 0000
>  exp = m-127 = -3, $2^{-3}$ = $\dfrac{1}{8}$ = 0.125
> f = 0x400000 -> $\dfrac{\dfrac{1}{8}}{2}$ = 0.0625
> sum = 0.125 + 0.0625 = 0.1875
* 0.00000 -> 0x3a000000 ->  0011 1010  0000 0000 - 0000 0000  0000 0000
>  m=01110100 f=0 exp = m-127 =

＊ 5.877472E-39 -> 0x00400000 -> 0000 0000  0100 0000 - 0000 0000  0000 0000
> m=00000000 f=0x400000

:::

:::spoiler  result:
ix0:02e2-0000,	r:800000,t:1000000,s0:2000000,q:1000000
ix0:c40000,			r:400000,t:2800000,s0:3000000,q:1800000
ix0:1880000,		r:200000,t:3400000,s0:3000000,q:1800000
ix0:3100000,		r:100000,t:3200000,s0:3000000,q:1800000
ix0:0,					r:80000, t:3100000,s0:3200000,q:1900000
ix0:0,					r:40000, t:3280000,s0:3200000,q:1900000
:::

* 動作說明 :
    * 首先使用 union 來做 float 和 int32_t 的型態轉換.
    * 接著做一些 exception handling. 包括: +-0, 0.0/0.0 (sNaN), 負浮點數, 以及 +INF 和 NaN.
    * 再來若 m 為'0', 則對做正規化: 將 fraction 左移知道 overflow, 並對m減掉相對應的值. 再修正 m 的偏移量 (-127)
    * 在開根號前, 先調整 m and f:
        * ix0　勘 取自 (ix0 & 0x007fffff) | 0x00800000 (0x00800000 是 補回 1.xxxx)
        * 若 m 為奇數, ix0 += ix0. 方便 m 開根號. (=> $2^{\dfrac{m/2}$)
    * m 開根號很簡單, m >> 1 即可. 再來處理小數部分():
        * 此時的 ix0 只有 3個 bytes, 63列, ix0 += ix0 用來補足位移量, 與後續的 r 對齊以便計算. (以後會 shift回來: line #90 )
        * r = 0x01000000, 與 ix0 對齊, 自左向右移動, 代表正在處理的位置. q 紀錄所得的結果.
        * 判斷是否 t <= ix0, 若成立則自 ix0 中減去 t (=s0+r) 並且 s0 累計此時的 2r.接著 q += r 紀錄結果.  
        * 74, 75 列分別對 ix0 和 r 雙向位移. 繼續下一個迴圈處理, 直到 r = 0.
    * 接著由 one +- tiny (很小的數)來判斷系統 rounding mode:
        * round up (one + tiny > one): q += 2. 因 q 最後一個 bit 會 shift 掉,因此 +2.
        * round down (one - tiny < one): 這裡不用處理,#99 列會 shift 掉.
        * round to nearest (one +- tiny = one): q += (q & 1).
    * 最後用 q & m 合成 ix0 (float):
        * 先對 q >> 1,因先前 ix0 += ix0 與 r 對齊,這裡 shift回來.
        * 再對 q + 0x3f000000. (原本是 0x3f800000, 但q 的 bit 23 = 1, m += 127 => q + 0x3f000000 (after shift).
        * 補上 m (<<23) 再由 INSERT_WORDS() 轉回 float 即為所求.

## 3 Consecutive Numbers Sum:

### consecutiveNumbersSum():
```c=
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
    }
    return ret;
}
```

$kx = N - \dfrac{(k-1)k}{2}$ and $K < sqrt(2N)$

觀察題意 for 迴圈結束的條件是 i >= x. 因此,答案只能選擇 "(d) 1-i", 否則迴圈無法結束.

* 動作說明 :
  * 首先不處理 N < 1 (return 0;). 接著 ret 是所求的值. x是連續正整數 i 是連續正整數個數 (即 K).
  * 迴圈 i 從 2 即開始可, x 依次遞減 (i-1) 若能被 i 整除,所求即 +1.
  * return ret 即為所求
  ![](https://i.imgur.com/BUyahZk.png)
