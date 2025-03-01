#include "list.h"
#include <stdio.h>
#include <stdlib.h>

void list_del(list_t *self)
{
    list_node_t *p = self->head, *next = NULL;
    while (p != NULL)
    {
        p->__del_data(p->data);
        next = p->next;
        free(p);
        p = next;
    }
    free(self);
}

void list_append(list_t *self, void *data, void (*del)(void *))
{
    list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    node->__del_data = del;
    if (self->head == NULL)
    {
        self->head = self->tail = node;
    }
    else
    {
        self->tail->next = node;
        node->prev = self->tail;
        self->tail = self->tail->next;
    }
}

void *list_iterator_data(list_iterator_t *self)
{
    if (self->curr != NULL)
        return self->curr->data;
    else
        return NULL;
}

void list_iterator_next(list_iterator_t *self)
{
    if (self->curr != NULL)
        self->curr = self->curr->next;
}

int list_iterator_end(list_iterator_t *self)
{
    return self->curr == NULL;
}

void list_iterator_del(list_iterator_t *self)
{
    free(self);
}

list_iterator_t *ListIterator_new(const list_t *self)
{
    list_iterator_t *iter = (list_iterator_t *)malloc(sizeof(list_iterator_t));
    iter->curr = self->head;
    iter->get = &list_iterator_data;
    iter->next = &list_iterator_next;
    iter->is_end = &list_iterator_end;
    iter->del = &list_iterator_del;
    return iter;
}

list_t *new_list()
{
    list_t *self = (list_t *)malloc(sizeof(list_t));
    self->head = NULL;
    self->tail = NULL;
    self->append = &list_append;
    self->get_iter = &ListIterator_new;
    self->del = &list_del;
    return self;
}
