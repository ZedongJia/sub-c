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
} List;

List *createList();
void freeList(List *list, void (*freeData)(void *));
void appendToList(List *list, void *data);
#endif