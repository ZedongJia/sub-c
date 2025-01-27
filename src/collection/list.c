#include "collection/list.h"
#include <stdio.h>
#include <stdlib.h>

List *createList()
{
    List *list = (List *)malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void freeList(List *list, void (*freeData)(void *))
{
    ListNode *p = list->tail, *prev = NULL;
    while (p != NULL)
    {
        prev = p->prev;
        freeData(p->data);
        free(p);
        p = prev;
    }
    free(list);
}

void appendToList(List *list, void *data)
{
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    if (list->head == NULL)
    {
        list->head = list->tail = node;
    }
    else
    {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = list->tail->next;
    }
}
