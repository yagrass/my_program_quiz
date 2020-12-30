# 2020q3 Homework4 (quiz4)
contributed by < ``yceugene`` >

## 1. Hamming Distance
* 題意: 計算兩個輸入整數的 Hamming Distance.
* 觀念: 計算兩個整數的相異位元數目即可.
* 答案：( C ) ^, x ^ y 來保留相異位元, 再以 __builtin_popcount() 計算位元數目即可!

### hammingDistance():
```c=
int hammingDistance(int x, int y) {
    return __builtin_popcount(x ^ y);
}
```

## 2. Tree Node
### treeAncestorCreate():
```c=
typedef struct {
    AAA;
    int max_level;
} TreeAncestor;

TreeAncestor *treeAncestorCreate(int n, int *parent, int parentSize)
{
    TreeAncestor *obj = malloc(sizeof(TreeAncestor));
    obj->parent = malloc(n * sizeof(int *));

    int max_level = 32 - __builtin_clz(n) + 1;
    for (int i = 0; i < n; i++) {
        obj->parent[i] = malloc(max_level * sizeof(int));
        for (int j = 0; j < max_level; j++)
            obj->parent[i][j] = -1;
    }
    for (int i = 0; i < parentSize; i++)
        obj->parent[i][0] = parent[i];

    for (int j = 1;; j++) {
        int quit = 1;
        for (int i = 0; i < parentSize; i++) {
            obj->parent[i][j] = obj->parent[i][j + BBB] == -1
                                    ? -1
                                    : obj->parent[obj->parent[i][j - 1]][j - 1];
            if (obj->parent[i][j] != -1) quit = 0;
        }
        if (quit) break;
    }
    obj->max_level = max_level - 1;
    return obj;
}
```
* 動作說明 :
    * 首先建立 TreeAncestor obj, 並且安排適當的記憶體.
    * 由 treeAncestorCreate() 裡的 obj->parent = malloc(n * sizeof(int *)) 可以看出 parent 是一個位址指向 n 個陣列的開頭, 而該陣列的內容是指標(位址), 指向 int. 因此 AAA 選擇 int **parent.
    * 接著兩層的 for loop allocate 一個 n x max_level 的二微 int 陣列, 並且將初值設為 '-1'.
    * 接著將 parent[i] 的值 [-1,0,0,1,1,2,2], 從'0'開始,依序填入obj->parent[i][0]直到 parentSize-1 為止. 經觀察,這裡的 parentSize 與 n 相同即可.
    * 最後, 將 parent[n][max_level] 中 其餘的 parent 填上. 規則是, 其左邊的parent若是-1,則維持-1,並終止該列迴圈,繼續下一列. 否則填入其相應的 parent. 以 parent[i][j] 為例,應填入 parent[parent[i][j-1]][j-1]. (取左邊的parent為新的node, 在取其j-1的值, 大約在表格的左上方).

### treeAncestorGetKthAncestor():
```c=
int treeAncestorGetKthAncestor(TreeAncestor *obj, int node, int k)
{
    int max_level = obj->max_level;
    for (int i = 0; i < max_level && node != -1; ++i)
        if (k & (CCC))
            node = obj->parent[node][i];
    return node;
}
```
* 動作說明 :
    * 經過測試, CCC 應選 1 << i.  迴圈中會將找到的 parent 指定給新的 node. 依序直到 i<max_level 或 node = -1 才停止並傳回 node.
    * 在parent[][]陣列中找到對應的 node 及 k parent並回傳 parent obj->parent[node][k]
    * 如果使用 for loop, parent 應該可以改成一維陣列即可.

### treeAncestorFree():
```c=
void treeAncestorFree(TreeAncestor *obj)
{
    for (int i = 0; i < obj->n; i++)
        free(obj->parent[i]);
    free(obj->parent);
    free(obj);
}
```
* 動作說明 :
    * 先逐一freeze obj->parent[] 的 memory, 接這 free obj->parent,最後 free obj. 按 malloc() 順序反向動作釋放記憶體.

## 3. Fizz buzz
* 動作說明 : 依題意, 需求得下表.
                    start       length
    div3               0           4
    div5               4           4
    div3 & div5        0           8
    others             8           2
    其中 length 已給定 length = (2 << div3) << div5; 符合所求.
    KK1, KK2, KK3 目的在計算 start position.
    * 當 others 時, start = 8 即為所求, 因此 KK1, KK2 此時不得>0.
    * 當 div3 = 1 時, start 必須為 '0', 因此, KK3 必須是足夠大的數字, 好讓 start 得到 0. 在這裡選 '2'.
    * 經比對,KK1 應為 div5, KK2應為 div3.

## 4. PAGE_QUEUES    
```c=
#include <stdint.h>
#define ffs32(x)  ((__builtin_ffs(x)) - 1)
size_t blockidx;
size_t divident = PAGE_QUEUES;
blockidx = offset >> ffs32(divident);
divident >>= ffs32(divident);
switch (divident) {
    CASES
}
```
* 動作說明 :
    * __builtin_ffs(x)-1 是二進位數字中, 右方有幾個 '0'. 如:
      __builtin_ffs(0x80)-1 = 7
      __builtin_ffs(0x08)-1 = 3
      __builtin_ffs(0x04)-1 = 2
      __buildin_ffs(0x01)-1 = 0
    * x >> ffs32(n) 即 x / $2^n$ 因此只需考慮質數 3, 5, 7
