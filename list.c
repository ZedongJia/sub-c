#include "list.h"
#include <stdio.h>
#include <stdlib.h>

void __List_del(List *list)
{
    ListNode *p = list->head, *next = NULL;
    while (p != NULL)
    {
        list->__delListNode(p->data);
        next = p->next;
        free(p);
        p = next;
    }
    free(list);
}

void __List_append(List *list, void *data)
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

List *createList(void (*delListNode)(void *))
{
    List *list = (List *)malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->__delListNode = delListNode;
    list->append = &__List_append;
    list->del = &__List_del;
    return list;
}