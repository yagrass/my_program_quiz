# 2020q3 Homework3 (quiz3)
contributed by < ``yceugene`` >
###### tags: `linux2020` `sysprog2020` `quiz`
[題目連結](https://hackmd.io/@sysprog/2020-quiz3)

## :memo: 目錄
[TOC]

## 1.有號整數的跨平台 ASR 實作
```c
int asr_i(signed int m, unsigned int n)
{
    const int logical = (((int) -1) OP1) > 0;
    unsigned int fixu = -(logical & (OP2));
    int fix = *(int *) &fixu;
    return (m >> n) | (fix ^ (fix >> n));
}
```

* 第 1 行是為了判斷編譯器的右移型態, 已知 -1 的二補數表示為 ``0xFFFFFFFF`` ，若``0xFFFFFFFF``在右移後得到正數，表示編譯器為邏輯右移（最高位補 0）；反之則為算術右移（符號位元右移），而位移結果的正負號判斷就是和 0 比大小，因此 ``OP1 = >>1``。（logical = 0 或 1 代表編譯器為邏輯右移或算術右移）
* 第 ２ 行是為了判斷 m 的正負號，只有當 m 為負數時才需考慮位移的正確性（正數位移，兩種右移型態都會從最高位元補 0，不影響結果正確性）。


    | 右移型態 | m 的正負號| 正確性 |
    | -------- | -------- | -------- |
    | 邏輯右移，即 ``logical==1`` | ``(m < 0)==1 `` | 錯誤|
    | 邏輯右移，即 ``logical==1`` | ``(m < 0)==0`` | 正確|
    | 算術右移，即 ``logical==0`` | ``(m < 0)==1`` | 正確 |
    | 算術右移，即 ``logical==0`` | ``(m < 0)==0`` | 正確 |

    由上表可知，我們只需要考慮邏輯右移且 ``m < 0`` 的情況，在此種情況下 ``fixu == 0xFFFFFFFF``, 其他三種情況下 ``fixu == 0x00000000``
* 第 3 行 ``int fix = *(int *) &fixu;`` 是將 fluxu 無號數）轉為 flux(有號數)
* 第 4 行是為了針對上述錯誤情況（負數的邏輯位移）做修正，從最高位元開始，將補 0 的位元全部改為 1。
* 解答：
    ``OP1 = >>1``
    ``OP2 = （m < 0）``

## 2.實作 Power of Four 的判斷
```c
bool isPowerOfFour(int num)
{
    return num > 0 && (num & (num - 1))==0 &&
           !(__builtin_ctz(num) OPQ);
}
```
想法： 若一個數字 num 為 4 的冪次方，則其二進制表示式中會有以下特性：
* 由於 num 必為 2 的冪次方，因此其二進制中恰有 1 個位元值為 1，其餘位元值為 0，而``(num & (num - 1))==0`` 就是用來判斷這個條件。
* num 的二進制中，位元值為 1 的位元，從該位元由其右邊一個 bit 一路向右延伸到底，一共會經過偶數個值為 0 的 bit，而 ``（__builtin_ctz(num) = & 0x1）`` 就是用來判斷 num 二進制式末尾是否有偶數個值為 0 的 bit。

* 解答：
    * ``OPQ = & 0x1``
## 3.LeetCode 1342. Number of Steps to Reduce a Number to Zero
* 題意: 針對輸入的 32bit int，需做多少次計算，才能將輸入變為 0。(計算條件為: 當正整數為偶數時，要對其做除以 2；當正整數為奇數時，要對其 -1。)
* 程式碼列表如下: (運用 gcc 內建函式實作，假設 int 寬度為 32 位元)
```
int numberOfSteps (int num)
{
    return num ? AAA + 31 - BBB : 0;
}
```

* 根據題意, 計算方式可以拆解為:
    * 當正整數為奇數時，把數值的 LSB 從 1 改為 0。
    * 當正整數為偶數時，找到數值最右邊不為 0 的位元，把數值邏輯右移直到該位元變成數值的 LSB 。

* 程式碼原理:
    因此總計算次數可以想成:
    1. **數值有多少個 1 ?** (這些 1 會依序被改成 0)
    可使用 __builtin_popcount(num) 做到。
    2. **要做幾次邏輯右移?**
    數值是 32 bit 正整數，最壞的情況是邏輯右移 31 次。但其實只要知道數值二進位表示中，從 MSB 往右數來第一個值為 1 的位元是哪一個位元即可，也就是計算 `31 - __builtin_clz(num)`。

* 答案":
    * AAA = `__builtin_popcount(num)`
    * BBB = `31 - __builtin_clz(num)`

## 4.64-bit GCD (greatest common divisor) 求值
* 題意:
* 原程式碼:
``` c
#include <stdint.h>
uint64_t gcd64(uint64_t u, uint64_t v) {
    if (!u || !v) return u | v;
    while (v) {
        uint64_t t = v;
        v = u % v;
        u = t;
    }
    return u;
}
```
* 等價實作程式碼:
``` c
#include <stdint.h>
uint64_t gcd64(uint64_t u, uint64_t v) {
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
```
### 程式原理:

如果 u、v 皆為偶數，就把 u、v 各除以 2，以 `shift` 紀錄除以 2 的次數。

`((u | v) & 1)` 是用來判斷 u、v 是否都為偶數。 (`& 1` 是用來取出數值的 LSB，LSB 為 1 等價於 他是奇數)
``` c
for (shift = 0; !((u | v) & 1); shift++) {
    u /= 2, v /= 2;
}
```

接者分別判斷 u、v 的奇偶性，若有一方為偶數，對他除以 2。
``` c
while (!(u & 1))
    u /= 2;
do {
    while (!(v & 1))
    :
    :
} while (XXX);
```

對 u、v 做輾轉相除法。
``` c
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
```

觀察程式碼會發現，每回合輾轉相除法算出的餘數會保留在 v 中，程式是透過餘數是否為 0 來判斷 gcd 是否計算完成，因此 `XXX = v`。程式執行完會回傳 gcd，gcd 的值是輾轉相除得到的商數 'u' 乘以前面是先除去的 2 的冪次方 (shift) 倍數，因此最後應回傳 `YYY = u << shift`。

## 5.bit array (bitset)
* 題意: 以 bitmap 方式處理輸入陣列 (每個元素會對應到 64 個 bit)，把所有值為 1 的 bit 對應的位置記下儲存到 `out`。兩分程式碼功能相同，但第二份效率較好。
* 程式碼 1:
``` c
#include <stddef.h>
size_t naive(uint64_t *bitmap, size_t bitmapsize, uint32_t *out)
{
    size_t pos = 0;
    for (size_t k = 0; k < bitmapsize; ++k) {
        uint64_t bitset = bitmap[k];
        size_t p = k * 64;
        for (int i = 0; i < 64; i++) {
            if ((bitset >> i) & 0x1)
                out[pos++] = p + i;
        }
    }
    return pos;
}
```

上述程式碼中，會逐一比對所有的位元 `if ((bitset >> i) & 0x1)`，判斷值是否為 1，這樣的效率比較差。


* 程式碼 2: (用 clz 改寫為效率更高且等價的程式碼)
``` c
#include <stddef.h>
size_t improved(uint64_t *bitmap, size_t bitmapsize, uint32_t *out)
{
    size_t pos = 0;
    uint64_t bitset;
    for (size_t k = 0; k < bitmapsize; ++k) {
        bitset = bitmap[k];
        while (bitset != 0) {
            uint64_t t = KKK;
            int r = __builtin_ctzll(bitset);
            out[pos++] = k * 64 + r;
            bitset ^= t;
        }
    }
    return pos;
}
```
上述程式碼是透過 __builtin_ctzll() 直接找到下一個 set bit 的位置，不用再逐一比較所有 bit，因此效率較好。

程式原理:
* __builtin_ctzll() 的作用是計算最右 set bit 到 LSB 之間有多少 0 bit。
* `int r = __builtin_ctzll(bitset);` 這裡的 r 是代表目前的 set bit 是第幾個位元。
* `bitset ^= t;` 是把剛剛紀錄到的 set bit 清除成 0，為了讓下一回合也能用 __builtin_ctzll(bitset) 做判斷。清除的方式是 `bitset & (bitset-1)`。 以 8 位元做考慮舉例如下: 假設 bitset 為 `0b0100 1000`，則: bitset-1 為 `0b0100 0111`；bitset & (bitset-1) 為 `0b0100 0000`。
