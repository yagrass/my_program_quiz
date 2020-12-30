#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MDebug printf

#define AAA int **parent
#define BBB -1
#define CCC 1 << i
// #define CCC (1<<i)

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
              : obj->parent[obj->parent[i][j-1]][j-1];
            if (obj->parent[i][j] != -1) quit = 0;
        }
        if (quit) break;
    }
    obj->max_level = max_level - 1;
    return obj;
}

int treeAncestorGetKthAncestor(TreeAncestor *obj, int node, int k)
{
    int max_level = obj->max_level;
    node = obj->parent[node][k-1];
    // for (int i = 0; i < max_level && node != -1; ++i) {
    //     printf("(%d)",i);
    //     if (k & (CCC)) {
    //         printf("<%d,%d|", node,i);
    //         node = obj->parent[node][i];
    //         printf("%d> \t", node);
    //     }
    // }
    return node;
}

void treeAncestorFree(TreeAncestor *obj)
{
    for (int i = 0; i < obj->max_level; i++)
        free(obj->parent[i]);
    free(obj->parent);
    free(obj);
}

void showAncestorTable(TreeAncestor *obj, int n)
{
    int max = 32 - __builtin_clz(n) + 1;
    printf("\n AncestorTable: \n");
    for(int i=0; i<n; i++) {
      printf("\n%d:\t", i);
      for(int j=0; j<max; j++)
        printf("%d\t", obj->parent[i][j]);
    }
    printf("\n");
}

int main()
{
  int aiAncestor[] = {-1, 0, 0, 1, 1, 2, 2};
  int aiGetAncestor[3][2] = {{3,1}, {5,2}, {6,3}};
  TreeAncestor *OBJ = treeAncestorCreate(sizeof(aiAncestor)/sizeof(int), aiAncestor, sizeof(aiAncestor)/sizeof(int) );

  showAncestorTable(OBJ, sizeof(aiAncestor)/sizeof(int));

  for(int i=0; i<3; i++) {
    int iRet = treeAncestorGetKthAncestor(OBJ, aiGetAncestor[i][0], aiGetAncestor[i][1]);
    printf("[%d,%d] get %d\n", aiGetAncestor[i][0],aiGetAncestor[i][1], iRet);
  }
}
