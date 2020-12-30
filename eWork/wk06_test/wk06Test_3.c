#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define MDebug  printf
#define A 7
#define B 4
#define C 5
#define D 9
#define SS1 node->next = *head
#define SS2 *head = node


#define cons(x, y) (struct llist[]){{y, x}}


struct llist {
    int val;
    struct llist *next;
};

void printList(struct llist *p)
{
  for(;p;p=p->next)
    MDebug("\t\tvalue=%d, next=%p\n", p->val, p->next);
}

void sorted_insert(struct llist **head, struct llist *node)
{
    if (!*head || (*head)->val >= node->val) {
        SS1;
        SS2;
        return;
    }
    struct llist *current = *head;
    while (current->next && current->next->val < node->val) {
        printf("\t\t\t\tc-next:%p,node:%p\n", current->next, node);
        current = current->next;
    }
    node->next = current->next;
    current->next = node;
    printf("\t\t\tc-next:%p,n-next:%p\n", current->next, node->next);
}

void sort(struct llist **head)
{
    struct llist *sorted = NULL;
    for (struct llist *current = *head; current;) {
        struct llist *next = current->next;
        sorted_insert(&sorted, current);
        MDebug("\t current=%p, sortted=%p\n", current, sorted);
        printList(sorted);
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
    printList(list);

    sort(&list);
    for (p = list; p; p = p->next)
        printf("%d", p->val);
    printf("\n");
    printList(list);
    return 0;
}
