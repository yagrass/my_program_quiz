# 2020q3 Homework2 (quiz2)
contributed by < ``yceugene`` >
###### tags: `linux2020` `sysprog2020`
題目連結： https://hackmd.io/@sysprog/2020-quiz2

## :memo: 目錄
[TOC]

## 1. ASCII 編碼
* 題意: 在明確界定 7 位元編碼的字元隸屬於 ASCII 的情況下，判斷指定的記憶體範圍內是否全是有效的 ASCII 字元。
* 觀念: 只需要判斷數值大小範圍是否藉於: 0~127，(若超過就不是有效的 ASCII 字元)。
* 完整 ASCII table 可參考: [ASCII Code - The extended ASCII table](https://www.ascii-code.com/)、[ASCII table , ascii codes](https://theasciicode.com.ar/ascii-printable-characters/at-sign-atpersand-ascii-code-64.html)

範例程式 1: 輸入字串記憶體位址和字串長度，針對字串長度，透過迭代**依序比較每個字元**的數值大小。
``` c
#include <stddef.h>
bool is_ascii(const char str[], size_t size)
{
    if (size == 0)
        return false;
    for (int i = 0; i < size; i++)
        if (str[i] & 0x80) /* i.e. (unsigned) str[i] >= 128 */
            return false;
    return true;
}
```

範例程式 2: 較快的版本，作法和前一段程式類似，但改成: **每次處理 64 bits 的資料 (即 word size)**:

``` c
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
bool is_ascii(const char str[], size_t size)
{
    if (size == 0)
        return false;
    int i = 0;
    while ((i + 8) <= size) {
        uint64_t payload;
        memcpy(&payload, str + i, 8);
        if (payload & MMM)
            return false;
        i += 8;
    }
    while (i < size) {
        if (str[i] & 0x80)
            return false;
        i++;
    }
    return true;
}
```
* 這邊 ``MMM`` 應填上: ``0x8080808080808080``
* 觀念:
    * 在第一支程式碼中，是逐一比對每個 char ，看值的大小是否超出 128 的範圍: ``str[i] & 0x80`` 這行的作用是將 char 和 ``1000 0000(二進制)`` 做 ``AND`` 運算，意思就是只留下 char 的最高位元，其他部分都變為 0。
    * 新的程式碼要在允許情況下，一次比對 64 位元的資料，意思就是一次處理 8 個 char，套用上面的做法，程式會逐一將將每 8 個 char，一併和 ``0x8080808080808080`` 做 ``AND`` 運算。
    * 為何用到 memcpy?


## 2. 開發解析器 (parser)
* 題意: 以不使用分支的方式，將十六進位表示的字串轉為數值 (不區分大小寫)。例如 `0xF` (大寫 `F` 字元) 和 `0xf` (小寫 `f` 字元) 都該轉換為數值 `15`。
* 已知 `'in'` 一定隸屬於 \`0`\``, `1`, `2`, …, `9` 及 `a`, `b`, `c`, …, `f` (小寫) 及 `A`, `B`, `C`, …, `F` (大寫) 這些字元。預期 `hexchar2val('F')` 和 `hexchar2val('f')` 回傳 `15`，且 `hexchar2val('0')` 回傳 `0`，其他輸入都有對應的數值。
* 程式碼:
    ``` c
    uint8_t hexchar2val(uint8_t in)
    {
        const uint8_t letter = in & MASK;
        const uint8_t shift = (letter >> AAA) | (letter >> BBB);
        return (in + shift) & 0xf;
    }
    ```
* 作答:
    * MASK: 0x40 (用來保留字串的第 5 個 bit 的值)
    * AAA: 3 (結合 AAA 和 BBB 來計算出數值 9 或 0)
    * BBB: 6
* 觀察:
先觀察 **數字字串**、**小寫字母**和**大寫字母**的二進制表示，以及其轉換後數值的二進制表示:

| 數字字串   | 字串的 16 進制表示 | 字串的二進制表示| => | 轉換後的數值(二進制表示) |
| -------- | -------- | -------- | -------- | -------- |
| `'0'` | `0x30` | `0011 0000` | => | `0000` |
| `'1'` | `0x31` | `0011 0001` | => | `0001` |
| `'2'` | `0x32` | `0011 0010` | => | `0010` |
| `'3'` | `0x33` | `0011 0011` | => | `0011` |
| `'4'` | `0x34` | `0011 0100` | => | `0100` |
| `'5'` | `0x35` | `0011 0101` | => | `0101` |
| `'6'` | `0x36` | `0011 0110` | => | `0110` |
| `'7'` | `0x37` | `0011 0111` | => | `0111` |
| `'8'` | `0x38` | `0011 1000` | => | `1000` |
| `'9'` | `0x39` | `0011 1001` | => | `1001` |

| 小寫字母   | 16 進制表示 | 字串的二進制表示| => | 轉換後的數值(二進制表示) |
| -------- | -------- | -------- | -------- | -------- |
| `'a'` | `0x61` | `0110 0001` | => | `1010` |
| `'b'` | `0x62` | `0110 0010` | => | `1011` |
| `'c'` | `0x63` | `0110 0011` | => | `1100` |
| `'d'` | `0x64` | `0110 0100` | => | `1101` |
| `'e'` | `0x65` | `0110 0101` | => | `1110` |
| `'f'` | `0x66` | `0110 0110` | => | `1111` |

| 大寫字母   | 16 進制表示 | 字串的二進制表示| => | 轉換後的數值(二進制表示) |
| -------- | -------- | -------- | -------- | -------- |
| `'A'` | `0x41` | `0100 0001` | => | `1010` |
| `'B'` | `0x42` | `0100 0010` | => | `1011` |
| `'C'` | `0x43` | `0100 0011` | => | `1100` |
| `'D'` | `0x44` | `0100 0100` | => | `1101` |
| `'E'` | `0x45` | `0100 0101` | => | `1110` |
| `'F'` | `0x46` | `0100 0110` | => | `1111` |

* 歸納三種字串的轉換如下:
    * **數字字串**:
    轉二進制表示時，其右邊 4 個 bits 恰好就是最終轉換出的數值大小。

    * **小寫字母**
    轉二進制表示時，其右邊 4 個 bits 轉成數值後再加 `9`，恰好就是最終轉換出的數值大小。(另外左半邊 4 個 bits 全為 `0110`)

    * **大寫字母**:
    轉二進制表示時，其右邊 4 個 bits 轉成數值後再加 `9`，恰好就是最終轉換出的數值大小。(另外左半邊 4 個 bits 全為 `0100`)

* 分析:
    * 程式碼最後一行回傳 ``(in + shift) & 0xf``，表示回傳時只考慮了``in + shift`` 二進制中的右邊 4 個 bits，這代表 ``in + shift`` 的右半部即為最終數值的二進制表示。
    * 再搭配上述表格，我們可以如下分析:
        * 針對數值字串轉二進制時，我們只需要把數值字串的左半邊清除成 `0000`，即可得到結果。因次在處理數值字串時， `shift` 須為 `0000`，程式才會輸出正確結果。
        * 針對英文母轉二進制時，我們需要把字串右半部加上 `9`，左半邊清除成 `0000`，即可得到結果。因次在處理字母字串時，不論大寫或小寫字母， `shift` 都必須是 `1001`，程式才會輸出正確結果。(剩餘目標是思考如何讓 `shift` 為 `1001`)
    * 考慮大小寫問題: 根據上述表格我們觀察到 "小寫字母的二進制表示中，左半邊全為 `0110`； 大寫字母的二進制表示中，左半邊全為 `0100`。

* 搭配程式碼，整個轉換過程可以這樣理解:
    * 把字母都視為大寫: 因此在``letter = in & MASK`` 這一行裡，MASK 為 `0100 0000`。
    * 轉換字母字串時，``shift`` 要設為數值 9 (即 `1001`)；轉換數字字串時，``shift`` 要設為數值 `0` (即 `0000`)。因此在``shift = (letter >> AAA) | (letter >> BBB)``這一行中: AAA 和 BBB 必須為 `3` 和 `6`(順序顛倒也可以)。
    * 最後回傳數值時，只留下右邊 4 個 bits，左邊 4 個 bits 要清為 0，因此回傳 ``(in + shift) & 0xf``。

## 3. 除法運算
* 題意:
    * 對已知的除數 `d (或 D)` 和被除數 `n` 做計算，求出餘數 `n - quotient * D`。
    * 利用乘法運算、位移操作、預先計算，來代替除法運算
* 程式碼:
``` c
const uint32_t D = 3;
#define M ((uint64_t)(UINT64_C(XXX) / (D) + 1))

/* compute (n mod d) given precomputed M */
uint32_t quickmod(uint32_t n)
{   uint64_t quotient = ((__uint128_t) M * n) >> 64;
    return n - quotient * D;
}
```
* 作答
XXX = 0xFFFFFFFFFFFFFFFF

* 程式原理:
    * 要先計算出商數 `quotient` 才能計算餘數。而`quotient`就是 `n/D` 的整數部分。

    * 根據提示`n/D` 可以表示成 $\frac{n}{D}=n \times \frac{\frac{2^{N}}{D}}{2^{N}}$，因此可將計算拆解成:
        * 預先計算: $2^{N} / d$
        * 計算: n * $2^{N} / d$ ，再將結果向右 shift `N bits`。 (搭配 `quotient = ((__uint128_t) M * n) >> 64;` 可知 `N` 為 `64`。
* 處理預先計算會 overflow 的問題:
已知 `N` 為 `64`，且 `M` 的型別為 `uint64_t`(上限為 $2^{64}-1$)，所以 `M` 不能直接設為 $2^{N} / d$。因此這裡把 M 設為 $(2^{N}-1)/d + 1$，因此 `XXX = 0xFFFFFFFFFFFFFFFF`。

> :warning: 關於為何M在計算時需要 +1, 分析如下:
> * 新增 printUI128() 將 __uint128_t 數字因出來觀察.
> * 新增 define NN, 同 M, 但不 "+1".
> * 加入 debub message, 將 M*n, quotient, 及 NN*n quotientNN 因出來觀察.
> * 修改後程式如下:
```c=
#define MDebug printf
const uint32_t D = 3;
#define M ((uint64_t)(UINT64_C(0xFFFFFFFFFFFFFFFF) / (D) +1 ))
#define MM ((uint64_t)(UINT64_C(0x1000000) / (D) ))
#define NN ((uint64_t)(UINT64_C(0xFFFFFFFFFFFFFFFF) / (D) ))
/* compute (n mod d) given precomputed M */

void printUI128(__uint128_t n)
{
  uint64_t m = (uint64_t)(n>> 64);
  // printf("%" PRIx64"-",m);
  printf("%016lx-",m);
  m = (n & (__uint128_t)0xFFFFFFFFFFFFFFFF);
  // printf("%" PRIx64,m);
  printf("%016lx",m);
}

uint32_t quickmod(uint32_t n)
{
    uint64_t quotient = ((__uint128_t)M * n) >> 64;
    uint64_t quotientNN = ((__uint128_t)NN * n) >> 64;

    MDebug("\tM= %lx, M*n= ", M);
    printUI128((__uint128_t)M*n);
    MDebug(", q=%lx\n",quotient);

    MDebug("\tNN=%lx, NN*n=", NN);
    printUI128((__uint128_t)NN*n);
    MDebug(", q=%lx\n",quotientNN);

    return n - quotient * D;
}

int main(int argc, char const *argv[])
{
  uint32_t aData[8] = {5, 55, 555, 12, 9, 100, 4, 3};
  uint32_t result;
  __uint128_t testValue = ((__uint128_t)0x1234567890ABCDEF<<8);
  printUI128(testValue);printf("\n");
  for(int i=0; i<8; i++) {
      result = quickmod(aData[i]);
      printf("mode of %d is: %d \n\n", aData[i], result);
  }
}
```

> * 輸出結果:
```c=
0000000000000012-34567890abcdef00
	M= 5555555555555556, M*n= 0000000000000001-aaaaaaaaaaaaaaae, q=1
	NN=5555555555555555, NN*n=0000000000000001-aaaaaaaaaaaaaaa9, q=1
mode of 5 is: 2

	M= 5555555555555556, M*n= 0000000000000012-555555555555557a, q=12
	NN=5555555555555555, NN*n=0000000000000012-5555555555555543, q=12
mode of 55 is: 1

	M= 5555555555555556, M*n= 00000000000000b9-0000000000000172, q=b9
	NN=5555555555555555, NN*n=00000000000000b8-ffffffffffffff47, q=b8
mode of 555 is: 0

	M= 5555555555555556, M*n= 0000000000000004-0000000000000008, q=4
	NN=5555555555555555, NN*n=0000000000000003-fffffffffffffffc, q=3
mode of 12 is: 0

	M= 5555555555555556, M*n= 0000000000000003-0000000000000006, q=3
	NN=5555555555555555, NN*n=0000000000000002-fffffffffffffffd, q=2
mode of 9 is: 0

	M= 5555555555555556, M*n= 0000000000000021-5555555555555598, q=21
	NN=5555555555555555, NN*n=0000000000000021-5555555555555534, q=21
mode of 100 is: 1

	M= 5555555555555556, M*n= 0000000000000001-5555555555555558, q=1
	NN=5555555555555555, NN*n=0000000000000001-5555555555555554, q=1
mode of 4 is: 1

	M= 5555555555555556, M*n= 0000000000000001-0000000000000002, q=1
	NN=5555555555555555, NN*n=0000000000000000-ffffffffffffffff, q=0
mode of 3 is: 0
```
> * 由輸出結果可以看到, 當被除數 (data) 是 3 的倍數時, NN*n 由於沒有 "+1", 因此在運算時,會發生誤差. 例如最後一個被除數 3, 其NN*n 是 0xffffffffffffffff. 因此, 在 >> 64之後變成了 0. (正確應是 1). 因此 "+1" 是必要的.
> * 同樣情形一樣會發生在其餘的除數上, 如 7, 11, ... etc.

## 4. (延伸測驗 3)，
* 題意: 判斷某個數值能否被指定的除數所整除 (即 n 是否為 D 的倍數，若是則回傳 1 ，若否則回傳 0)
* 在 `D` 和 `M` 都沿用的狀況下 (D = 3,  M = `0xFFFFFFFFFFFFFFFF / 3 + 1 = 0x5555555555555556`)，程式碼如下:
``` c
bool divisible(uint32_t n)
{
    return n * M <= YYY;
}
```
* 注意條件: n 的資料型別為 uint32_t，M 的資料型別為 uint64_t，又 M = 0x5555555555555556，故 `n < M`
* 根據 n mod 3 的特性，n 有三種可能: 3k, 3k + 1, 3k + 2 (k 為整數)，每種對應的 n * M 為:
    1. 若 n = 3k, n * M = 3k * `0x5555555555555556` = 3k * (`0x5555555555555555` + `0x0000000000000001`) = k * (`0xFFFFFFFFFFFFFFFF` + `0x0000000000000003`) = k * `0x0000000000000002` = 2k (小於 M)
    2. 若 n = 3k + 1, n * M = (3k + 1) * M = 3kM + M = 2k + M (大於 M)
    3. 若 n = 3k + 2, n * M = (3k + 2) * M = 3kM + 2M = 2k + 2M, 即 M < n * M (大於 M)
* 因為 n 不會比 M 大，因此可以搭配上述特性，比較 n*M 的計算結果和 M 的大小關係，判斷 n 是否整除 3
* 問題: 但我不清楚，為何要選 M-1 不是 M。

## 5. 英文字母大小寫轉換
`strlower` 的 in-place 實作如下:
``` c
#include <ctype.h>
#include <stddef.h>
/* in-place implementation for converting all characters into lowercase. */
void strlower(char *s, size_t n)
{
    for (size_t j = 0; j < n; j++) {
        if (s[j] >= 'A' && s[j] <= 'Z')
            s[j] ^= 1 << 5;
        else if ((unsigned) s[j] >= '\x7f') /* extended ASCII */
            s[j] = tolower(s[j]);
    }
}
```
實作向量化的 `strlower`:
``` c
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>

#define PACKED_BYTE(b) (((uint64_t)(b) & (0xff)) * 0x0101010101010101u)

/* vectorized implementation for in-place strlower */
void strlower_vector(char *s, size_t n)
{
    size_t k = n / 8;
    for (size_t i = 0; i < k; i++, s += 8) {
        uint64_t *chunk = (uint64_t *) s;
        if ((*chunk & PACKED_BYTE(VV1)) == 0) { /* is ASCII? */
            uint64_t A = *chunk + PACKED_BYTE(128 - 'A' + VV2);
            uint64_t Z = *chunk + PACKED_BYTE(128 - 'Z' + VV3);
            uint64_t mask = ((A ^ Z) & PACKED_BYTE(VV4)) >> 2;
            *chunk ^= mask;
        } else
            strlower(s, 8);
    }

    k = n % 8;
    if (k)
        strlower(s, k);
}
```
* 在 64 位元處理架構中，以 word 為單位來處理字串，因此要把輸入字串拆成每 8 個一組 (一個 char 佔一位元)。 `#define PACKED_BYTE(b) (((uint64_t)(b) & (0xff)) * 0x0101010101010101u)` 這行的作用是把 8 位元的資料擴充成內容重複的 64 位元，在 strlower_vector() 的程式碼中，我們需要一次判斷 8 個字元是否為 ASCII 字元 (根據 [測驗1]() 我們知道可以透過 `str[i] & 0x80` 來判斷一個字元是否落在 ASCII 範圍內)，這裡的作法是利用 `#define PACKED_BYTE(b)` 將 `0x80` 擴展成 `0x0808080808080808`。因此 `if ((*chunk & PACKED_BYTE(VV1)) == 0)` 中，`VV1` 應填上 `0x80`。

* 解讀以下程式碼
``` c
uint64_t A = *chunk + PACKED_BYTE(128 - 'A' + VV2);
uint64_t Z = *chunk + PACKED_BYTE(128 - 'Z' + VV3);
uint64_t mask = ((A ^ Z) & PACKED_BYTE(VV4)) >> 2;
*chunk ^= mask;
```
先觀察 `128 - 'A' + VV2` 和 `128 - 'Z' + VV3`，推測這段程式碼應該是在判斷字元是否落於 `'A' ~ 'Z'` 的範圍。

把 ASCII 字元，分成三個範圍: 小於 'A', 大於等於 'A' 且 小於等於 'Z', 大於 'Z'。 從三個範圍中各取一個字元放到下方表格。
| char | Decimal| => | 與 127 的距離 |
| -------- | -------- | -------- | -------- |
| `'c'` (大於 'Z') | `99` | => | `28` |
| `'Z'` | `90` | => | `37` (128 - `'Z'` + 1) |
| `'C'` (大於等於 'A' 且 小於等於 'Z')| `67` | => | `60` |
| `'A'` | `65` | => | `62` (128 - `'A'` + 1)|
| `' '` (小於 'A')| `32` | => | `95` |

要判斷一個字元是否落於 `'A' ~ 'Z'` 的範圍，除了直接比較字元和 'A'、'Z' 的大小以外，也可以考慮把字元加上 `127 - 'A'` 及 `127 - 'Z'`，判斷結果是否會超過 ASCII 的範圍 (>127)。

* 若輸入字元 > 'A'，則輸入字元 + `127 - 'A'` 會大於 127，其二進制表示中最左 bit 會是1。
* 若輸入字元 >= 'A'，則輸入字元 + `127 - 'A' + 1` 會大於 127 (**即輸入字元 + `128 - 'A'`**)，其二進制表示中最左 bit 會是1。
* 若輸入字元 < 'Z'，則輸入字元 + `127 - 'Z'` 會小於 127，其二進制表示中最左 bit 會是 0。
* 若輸入字元 <= 'Z'，則輸入字元 + `127 - 'Z'` 會小等於 127 (**即輸入字元 + `128 - 'Z' - 1`**)，其二進制表示中最左 bit 會是 0。

因此 `'A' ~ 'Z'` 範圍判斷方式應為: 輸入字元分別加上 `128 - 'A'` 及 `128 - 'Z' - 1`。

In order to set bit 5 for each char in 64bits word, 0x80 (VV4) was required to & (A ^ Z) and followed by >> 2.

> * examination: modify source code to show more debug information as below:
```c
/* vectorized implementation for in-place strlower */
void strlower_vector(char *s, size_t n)
{
    size_t k = n / 8;
    for (size_t i = 0; i < k; i++, s += 8) {
        uint64_t *chunk = (uint64_t *) s;
        char acBuff[9] = {0,0,0,0,0,0,0,0,0};
        memcpy((void*)acBuff, (void*)s, 8);
        MDebug("<%s> if(%lX==0)\n",
               acBuff, (*chunk & PACKED_BYTE(VV1)) );

        if ((*chunk & PACKED_BYTE(VV1)) == 0) { /* is ASCII? */
            uint64_t A = *chunk + PACKED_BYTE(128 - 'A' + VV2);
            uint64_t Z = *chunk + PACKED_BYTE(128 - 'Z' + VV3);
            uint64_t mask = ((A ^ Z) & PACKED_BYTE(VV4)) >> 2;
            *chunk ^= mask;
            memcpy((void*)acBuff, (void*)s, 8);
            MDebug("\tA=%lx, Z=%lx, mask=%016lx, *chunk=%lx\n\t=>%s\n\n", A, Z, mask, *chunk, acBuff);
        } else
            strlower(s, 8);
    }

    k = n % 8;
    if (k)
        strlower(s, k);
}
```


> * the execution data can be found below:

```c
<This eBo> if(0==0)
	A=ae81a45fb2a8a793, Z=94678a45988e8d79, mask=0020000000000020, *chunk=6f62652073696874
	=>this ebo

<ok is fo> if(0==0)
	A=aea55fb2a85faaae, Z=948b45988e459094, mask=0000000000000000, *chunk=6f66207369206b6f
	=>ok is fo

<r the us> if(0==0)
	A=b2b45fa4a7b35fb1, Z=989a458a8d994597, mask=0000000000000000, *chunk=7375206568742072
	=>r the us

<e of any> if(0==0)
	A=b8ada05fa5ae5fa4, Z=9e9386458b94458a, mask=0000000000000000, *chunk=796e6120666f2065
	=>e of any

<one anyw> if(0==0)
	A=b6b8ada05fa4adae, Z=9c9e9386458a9394, mask=0000000000000000, *chunk=77796e6120656e6f
	=>one anyw

<here at > if(0==0)
	A=5fb3a05fa4b1a4a7, Z=459986458a978a8d, mask=0000000000000000, *chunk=2074612065726568
	=>here at

<no cost > if(0==0)
	A=5fb3b2aea25faead, Z=4599989488459493, mask=0000000000000000, *chunk=2074736f63206f6e
	=>no cost

<and with> if(0==0)
	A=a7b3a8b65fa3ada0, Z=8d998e9c45899386, mask=0000000000000000, *chunk=6874697720646e61
	=>and with

< almost > if(0==0)
	A=5fb3b2aeacaba05f, Z=4599989492918645, mask=0000000000000000, *chunk=2074736f6d6c6120
	=> almost

<no restr> if(0==0)
	A=b1b3b2a4b15faead, Z=9799988a97459493, mask=0000000000000000, *chunk=7274736572206f6e
	=>no restr

<ictions > if(0==0)
	A=5fb2adaea8b3a2a8, Z=459893948e99888e, mask=0000000000000000, *chunk=20736e6f69746369
	=>ictions

<whatsoev> if(0==0)
	A=b5a4aeb2b3a0a7b6, Z=9b8a949899868d9c, mask=0000000000000000, *chunk=76656f7374616877
	=>whatsoev

<er.  You> if(0==0)
	A=b4ae985f5f6db1a4, Z=9a947e454553978a, mask=0000200000000000, *chunk=756f7920202e7265
	=>er.  you

< may cop> if(0==0)
	A=afaea25fb8a0ac5f, Z=959488459e869245, mask=0000000000000000, *chunk=706f632079616d20
	=> may cop

<y it, gi> if(0==0)
	A=a8a65f6bb3a85fb8, Z=8e8c4551998e459e, mask=0000000000000000, *chunk=6967202c74692079
	=>y it, gi

<ve it aw> if(0==0)
	A=b6a05fb3a85fa4b5, Z=9c8645998e458a9b, mask=0000000000000000, *chunk=7761207469206576
	=>ve it aw

<ay or re> if(0==0)
	A=a4b15fb1ae5fb8a0, Z=8a97459794459e86, mask=0000000000000000, *chunk=657220726f207961
	=>ay or re

<-use it > if(0==0)
	A=5fb3a85fa4b2b46c, Z=45998e458a989a52, mask=0000000000000000, *chunk=207469206573752d
	=>-use it

<under th> if(0==0)
	A=a7b35fb1a4a3adb4, Z=8d9945978a89939a, mask=0000000000000000, *chunk=6874207265646e75
	=>under th

<e terms > if(0==0)
	A=5fb2acb1a4b35fa4, Z=459892978a99458a, mask=0000000000000000, *chunk=20736d7265742065
	=>e terms

<of the P> if(0==0)
	A=8f5fa4a7b35fa5ae, Z=75458a8d99458b94, mask=2000000000000000, *chunk=702065687420666f
	=>of the p

<roject G> if(0==0)
	A=865fb3a2a4a9aeb1, Z=6c4599888a8f9497, mask=2000000000000000, *chunk=67207463656a6f72
	=>roject g

<utenberg> if(0==0)
	A=a6b1a4a1ada4b3b4, Z=8c978a87938a999a, mask=0000000000000000, *chunk=677265626e657475
	=>utenberg

< License> if(0==0)
	A=a4b2ada4a2a88b5f, Z=8a98938a888e7145, mask=0000000000002000, *chunk=65736e6563696c20
	=> license

< include> if(0==0)
	A=a4a3b4aba2ada85f, Z=8a899a9188938e45, mask=0000000000000000, *chunk=6564756c636e6920
	=> include

<d with t> if(0==0)
	A=b35fa7b3a8b65fa3, Z=99458d998e9c4589, mask=0000000000000000, *chunk=7420687469772064
	=>d with t

<his eBoo> if(0==0)
	A=aeae81a45fb2a8a7, Z=9494678a45988e8d, mask=0000200000000000, *chunk=6f6f626520736968
	=>his eboo

<k or onl> if(0==0)
	A=abadae5f99805faa, Z=919394457f664590, mask=0000000020200000, *chunk=6c6e6f207a61206b
	=>k az onl

<ine at w> if(0==0)
	A=b65fb3a05fa4ada8, Z=9c459986458a938e, mask=0000000000000000, *chunk=7720746120656e69
	=>ine at w

<ww.guten> if(0==0)
	A=ada4b3b4a66db6b6, Z=938a999a8c539c9c, mask=0000000000000000, *chunk=6e657475672e7777
	=>ww.guten

<berg.net> if(0==0)
	A=b3a4ad6da6b1a4a1, Z=998a93538c978a87, mask=0000000000000000, *chunk=74656e2e67726562
	=>berg.net

this ebook is for the use of anyone anywhere at no cost and with almost no restrictions whatsoever.  you may copy it, give it away or re-use it under the terms of the project gutenberg license included with this ebook or online at www.gutenberg.net
```

> :warning: the information above, low byte was displayed first for each long word. For example, 'mask=0020000000000020' in the second line, means 'T' and 'B' upper case were found from 'This eBo' string. 'T' is the first char but displayed last, while B was the 7th char, but displayed as 2nd byte. This is litte-endian feature of Intel processor.

## 6.  LeetCode 137. Single Number II
### 關於 KKK = ?
觀察返回值 lower 其初值是 0, 第一個操作 ^= 之後, 設為輸入值, 第二個動作 &= 是清除, 因此:
* (a) higher: 清除為0, 這項不對.
* (b) lower: 無動作, 也不對.
* (d) ~lower: 清除 lower 自己, 這項不對。
* (e) 0xFFFFFFFF: 無動作, 也不對.
因此選 ( C) ~higher.

### 關於 JJJ = ?
同 lower 其初值是 0, 第一個操作 ^= 之後, 設為輸入值, 第二個動作 &= 是清除, 因此:
* (a) higher: 無動作, 不對.
* (b) lower: 保留 lower,清除其餘的 bits, 怪怪的!
* (c) ~higher: 清除 higher 自己, 這項不對。
* (e) 0xFFFFFFFF: 無動作, 也不對.
因此選 (d) ~lower.

加上 MDebug 來偵錯, 完整程式如下:
``` c
#define MDebug printf
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
```
執行結果如下:
```
2 	higher:0x00000000, lower:0x00000002
2 	higher:0x00000002, lower:0x00000000
2 	higher:0x00000000, lower:0x00000000
3 	higher:0x00000000, lower:0x00000003
Output: 3
0 	higher:0x00000000, lower:0x00000000
1 	higher:0x00000000, lower:0x00000001
0 	higher:0x00000000, lower:0x00000001
1 	higher:0x00000001, lower:0x00000000
0 	higher:0x00000001, lower:0x00000000
1 	higher:0x00000000, lower:0x00000000
99 	higher:0x00000000, lower:0x00000063
Output: 99
21 	higher:0x00000000, lower:0x00000015
66 	higher:0x00000000, lower:0x00000057
66 	higher:0x00000042, lower:0x00000015
66 	higher:0x00000000, lower:0x00000015
Output: 21
```
