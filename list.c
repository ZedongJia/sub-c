#include "list.h"
#include <stdio.h>
#include <stdlib.h>

void __List_del(struct List *list)
{
    struct ListNode *p = list->__head, *next = NULL;
    while (p != NULL)
    {
        p->__del_data(p->__data);
        next = p->__next;
        free(p);
        p = next;
    }
    free(list);
}

void __List_append(struct List *list, void *data, void (*del)(void *))
{
    struct ListNode *node = (struct ListNode *)malloc(sizeof(struct ListNode));
    node->__data = data;
    node->__next = NULL;
    node->__prev = NULL;
    node->__del_data = del;
    if (list->__head == NULL)
    {
        list->__head = list->__tail = node;
    }
    else
    {
        list->__tail->__next = node;
        node->__prev = list->__tail;
        list->__tail = list->__tail->__next;
    }
}

struct List *create_list()
{
    struct List *list = (struct List *)malloc(sizeof(struct List));
    list->__head = NULL;
    list->__tail = NULL;
    list->append = &__List_append;
    list->del = &__List_del;
    return list;
}

void *__ListIterator_data(struct ListIterator *iter)
{
    if (iter->__curr != NULL)
        return iter->__curr->__data;
    else
        return NULL;
}

void __ListIterator_next(struct ListIterator *iter)
{
    if (iter->__curr != NULL)
        iter->__curr = iter->__curr->__next;
}

int __ListIterator_end(struct ListIterator *iter)
{
    return iter->__curr == NULL;
}

void __ListIterator_del(struct ListIterator *iter)
{
    free(iter);
}

struct ListIterator *create_list_iterator(struct List *list)
{
    struct ListIterator *iter = (struct ListIterator *)malloc(sizeof(struct ListIterator));
    iter->__curr = list->__head;
    iter->data = &__ListIterator_data;
    iter->next = &__ListIterator_next;
    iter->end = &__ListIterator_end;
    iter->del = &__ListIterator_del;
    return iter;
}