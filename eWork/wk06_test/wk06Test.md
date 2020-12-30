# 2020q3 Homework6 (quiz6)
contributed by < ``yceugene`` >

## 1. fp32tobf16:

### fp32tobf16():
```c=
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
    *pr &= BB1;
    r /= 256;
    y = x + r;

    *py &= BB2;
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
    }

    return 0;
}
```
3.140625=40490000
3.140625=40490000
1.200000=3f99999a
1.203125=3f9a0000
2.310000=4013d70a
2.312500=40140000
3.460000=405d70a4
3.453125=405d0000
5.630000=40b428f6
5.625000=40b40000


* 動作說明 :
    * 為方便位元運算,line #3 先將 float 轉成 int. 接著,自 IEEE 754 float32 中取出 exp 及 man.
    * line #7 ~ #10, 處理 exception handling, 分別將 zero, infinity 及 Nan 狀況排除.
    * 接著根據 exp (*pr &= 0xff800000 含 sign bit) 算出 runding number (即 r, float of *pr)　/= 256. (對 floating number 只能用除, 這裡不能用 shift).
    * 這個 r 會出現在 float 32 的 bit 15, 也就是 bfloat16 外部的第一個 bit. line #17,　y = x + r, 就得到 rounding to nearest (進位或捨棄) bit 15 ~ 0.
    * 最後返回前 *py &= 0xFFFF0000 將 y 的低 16 位元清除為 '0'. (BB2 = 0xFFFF0000)

float32 to bfloat16 example:
![](https://i.imgur.com/kT8p5ou.png)

## 2. ring buffer:

### ring buffer program:
```c=
#define RINGBUF_DECL(T, NAME) \
    typedef struct {          \
        int size;             \
        int start, end;       \
        T *elements;          \
    } NAME

#define RINGBUF_INIT(BUF, S, T)             \
    {                                       \
        static T static_ringbuf_mem[S + 1]; \
        BUF.elements = static_ringbuf_mem;  \
    }                                       \
    BUF.size = S;                           \
    BUF.start = 0;                          \
    BUF.end = 0;

#define NEXT_START_INDEX(BUF) \
    (((BUF)->start != (BUF)->size) ? ((BUF)->start + RB1) : 0)
#define NEXT_END_INDEX(BUF) (((BUF)->end != (BUF)->size) ? ((BUF)->end + RB2) : 0)

#define is_ringbuf_empty(BUF) ((BUF)->end == (BUF)->start)
#define is_ringbuf_full(BUF) (NEXT_END_INDEX(BUF) == (BUF)->start)

#define ringbuf_write_peek(BUF) (BUF)->elements[(BUF)->end]
#define ringbuf_write_skip(BUF)                   \
    do {                                          \
        (BUF)->end = NEXT_END_INDEX(BUF);         \
        if (is_ringbuf_empty(BUF))                \
            (BUF)->start = NEXT_START_INDEX(BUF); \
    } while (0)

#define ringbuf_read_peek(BUF) (BUF)->elements[(BUF)->start]
#define ringbuf_read_skip(BUF) (BUF)->start = NEXT_START_INDEX(BUF);

#define ringbuf_write(BUF, ELEMENT)        \
    do {                                   \
        ringbuf_write_peek(BUF) = ELEMENT; \
        ringbuf_write_skip(BUF);           \
    } while (0)

#define ringbuf_read(BUF, ELEMENT)        \
    do {                                  \
        ELEMENT = ringbuf_read_peek(BUF); \
        ringbuf_read_skip(BUF);           \
    } while (0)
#include <assert.h>

RINGBUF_DECL(int, int_buf);

int main()
{
    int_buf my_buf;
    RINGBUF_INIT(my_buf, 2, int);
    assert(is_ringbuf_empty(&my_buf));

    ringbuf_write(&my_buf, 37);
    ringbuf_write(&my_buf, 72);
    assert(!is_ringbuf_empty(&my_buf));

    int first;
    ringbuf_read(&my_buf, first);
    assert(first == 37);

    int second;
    ringbuf_read(&my_buf, second);
    assert(second == 72);

    return 0;
}    
```
* 動作說明 :
    * 測試程式一開始,先定義一個 int_buf structur, 其成員如 line #1 ~ #6.
    * 一進到 main(), 宣告結構變數 my_buff, 然後進行初始化:
      * 宣告一個 static int 陣列 static_ringbuf_men[3] (三個成員), 並將位置存在 elements.
      * 接著 size = 2, start = 0 and end = 0;
    * 接著 assert() 確保 my_buf 結構是空的. 判斷方式是: 其成員 start = end.
    * 再來,連續寫入 37 及 72 到 buffer, 接著 assert() 確保 buffer 現在不是空的.
    * 接著依序將寫入的值讀出,並判斷是不是原先寫入的 37 及 72.
    * 要注意的是:
      * 寫入時是從 end 寫入,讀出時是從 start 讀出.
      * 寫入或讀出之後, end 和 start 都必須移到下個位置, 移動的方式,基本上是 +1,當然要避免 overflow (== size, 這裡是 2). 因此, RB1 & RB2 在這裡都選 1.

## 3 靜態初始化的 singly-linked list

### singly-linked list 實作:
```c=
#include <stdio.h>

/* clang-format off */
#define cons(x, y) (struct llist[]){{y, x}}
/* clang-format on */

struct llist {
    int val;
    struct llist *next;
};

void sorted_insert(struct llist **head, struct llist *node)
{
    if (!*head || (*head)->val >= node->val) {
        SS1;
        SS2;
        return;
    }
    struct llist *current = *head;
    while (current->next && current->next->val < node->val)
        current = current->next;
    node->next = current->next;
    current->next = node;
}

void sort(struct llist **head)
{
    struct llist *sorted = NULL;
    for (struct llist *current = *head; current;) {
        struct llist *next = current->next;
        sorted_insert(&sorted, current);
        current = next;
    }
    *head = sorted;
}

int main()
{
    struct llist *list = cons(cons(cons(cons(NULL, A), B), C), D);
    struct llist *p;
    for (p = list; p; p = p->next)
        printf("%d", p->val);
    printf("\n");

    sort(&list);
    for (p = list; p; p = p->next)
        printf("%d", p->val);
    printf("\n");
    return 0;
}
```
### 執行結果:
```c=
9547
		value=9, next=0x7ffc79566b60
		value=5, next=0x7ffc79566b50
		value=4, next=0x7ffc79566b40
		value=7, next=(nil)
	 current=0x7ffc79566b70, sortted=0x7ffc79566b70
		value=9, next=(nil)
	 current=0x7ffc79566b60, sortted=0x7ffc79566b60
		value=5, next=0x7ffc79566b70
		value=9, next=(nil)
	 current=0x7ffc79566b50, sortted=0x7ffc79566b50
		value=4, next=0x7ffc79566b60
		value=5, next=0x7ffc79566b70
		value=9, next=(nil)
	 current=0x7ffc79566b40, sortted=0x7ffc79566b50
		value=4, next=0x7ffc79566b60
		value=5, next=0x7ffc79566b40
		value=7, next=0x7ffc79566b70
		value=9, next=(nil)
4579
		value=4, next=0x7ffc79566b60
		value=5, next=0x7ffc79566b40
		value=7, next=0x7ffc79566b70
		value=9, next=(nil)
```
* 動作說明 :
    * 首先建立一個 linked list: 9, 5, 4, 7, 以 9 開始,依序連結到下個元素直到最後一個 7.接著將 linked list 輸出到螢幕
    * 再將 linked list 由小到大排序, 並傳回新的 *head. (注意, 因為 list 是 point to struct llist, 所以必須以 &list當作參數傳入 sort().
    * 排序完成再將 linked list 輸出到螢幕
    * 要注意的是:
      * 連續的 cons() 組成 linked list. 由最內部的 cons() 開始,第一個next是 NULL, 然後依序將其地址連結到上一個成員的 next.因此,最裡面的 A 是最後面的 7, 接著 B = 4, C = 5, D = 9.
      * 在 sort() 裡面, sorted 用來存放排序完成的根, 剛開始是 NULL. 在排序結束前會指定給 *head 來傳回到呼叫函式.
      * for 迴圈裡, current 是當前處理的成員, 初值是 *head. next 則是下個成員.
      * 接著呼叫 sorted_insert() 進行排序.
      * 在 sorted_insert() 裡, 一開始先判對 *head 是否為 NULL 或 其 value 是否 >= next 的 value, 若是則將 *head 對調 (node->next 指向 *head). 並結束呼叫.
      * 否則,用 while 迴圈由 current->next 開始 (除非他是NULL) 尋找 current->next->value >= node-val 的成員, 並將它與 node 互換.

## 4 Find the Duplicate Number

### 程式碼
```c=
int findDuplicate(int *nums, int numsSize)
{
    int res = 0;
    const size_t log_2 = 8 * sizeof(int) - __builtin_clz(numsSize);
    for (size_t i = 0; i < log_2; i++) {
        int bit = 1 << i;
        int c1 = 0, c2 = 0;
        for (size_t k = 0; k < numsSize; k++) {
            if (k & bit)
                ++c1;
            if (nums[k] & bit)
                ++c2;
        }
        if (CCC)
            res += bit;
    }
    return res;
}
```
* 動作說明 :
    * log_2 是有效位元數,代表需要檢查的位元. 在 for 迴圈裡 bit 自 1 開始向左 shift, 依序檢查每有效位元.
    * 第二層的 for 迴圈依照 bit (自 1 開始)計算 c1 及 c2:
      * c1 是在數字不重複的情形下, 對應 bit的重複次數.因此, 他是, 陣列長度 n 的變數.與輸入的數字無關!
      * c2 是輸入的數字中該 bit 出現的次數.
    * 因此在內層迴圈結束後,只要檢查 c2 是否大於 c1 即可判定該 bit 是否曾重複出現過? 如果是, 就將該 bit 記錄起來 (res). 最後 res 即為所求.

  duplicated number example: {1,3,4,2,2}
![](https://i.imgur.com/hj0YRTW.png)

  diplicated number example: {1,3,2,2,2}
  ![](https://i.imgur.com/NKEX7NR.png)
