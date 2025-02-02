#ifndef LIST
#define LIST
typedef struct ListNode
{
    void *data;
    struct ListNode *next;
    struct ListNode *prev;
} ListNode;

typedef struct List
{
    ListNode *head;
    ListNode *tail;
    void (*__delListNode)(void *);
    void (*append)(struct List *list, void *data);
    void (*del)(struct List *list);
} List;

List *createList(void (*delListNode)(void *));
#endif