#include "list.h"
#include <stdio.h>
#include <stdlib.h>

void __List_del(struct List *self)
{
    struct ListNode *p = self->__head, *next = NULL;
    while (p != NULL)
    {
        p->__del_data(p->__data);
        next = p->__next;
        free(p);
        p = next;
    }
    free(self);
}

void __List_append(struct List *self, void *data, void (*del)(void *))
{
    struct ListNode *node = (struct ListNode *)malloc(sizeof(struct ListNode));
    node->__data = data;
    node->__next = NULL;
    node->__prev = NULL;
    node->__del_data = del;
    if (self->__head == NULL)
    {
        self->__head = self->__tail = node;
    }
    else
    {
        self->__tail->__next = node;
        node->__prev = self->__tail;
        self->__tail = self->__tail->__next;
    }
}

struct List *new_list()
{
    struct List *self = (struct List *)malloc(sizeof(struct List));
    self->__head = NULL;
    self->__tail = NULL;
    self->append = &__List_append;
    self->del = &__List_del;
    return self;
}

void *__ListIterator_data(struct ListIterator *self)
{
    if (self->__curr != NULL)
        return self->__curr->__data;
    else
        return NULL;
}

void __ListIterator_next(struct ListIterator *self)
{
    if (self->__curr != NULL)
        self->__curr = self->__curr->__next;
}

int __ListIterator_end(struct ListIterator *self)
{
    return self->__curr == NULL;
}

void __ListIterator_del(struct ListIterator *self)
{
    free(self);
}

struct ListIterator *new_list_iterator(struct List *self)
{
    struct ListIterator *iter = (struct ListIterator *)malloc(sizeof(struct ListIterator));
    iter->__curr = self->__head;
    iter->data = &__ListIterator_data;
    iter->next = &__ListIterator_next;
    iter->end = &__ListIterator_end;
    iter->del = &__ListIterator_del;
    return iter;
}