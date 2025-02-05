#ifndef LIST
#define LIST
struct ListNode
{
    void *__data;
    struct ListNode *__next;
    struct ListNode *__prev;
    void (*__del_data)(void *);
};

struct ListIterator
{
    struct ListNode *__curr;
    void *(*data)(struct ListIterator *);
    void (*next)(struct ListIterator *);
    int (*end)(struct ListIterator *);
    void (*del)(struct ListIterator *);
};

struct List
{
    struct ListNode *__head;
    struct ListNode *__tail;
    void (*append)(struct List *list, void *data, void (*del)(void *));
    void (*del)(struct List *list);
};

struct List *create_list();
struct ListIterator *create_list_iterator(struct List *list);
#endif