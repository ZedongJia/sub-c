#ifndef LIST
#define LIST
typedef struct list_node
{
    void *data;
    struct list_node *next;
    struct list_node *prev;
    void (*__del_data)(void *data);
} list_node_t;

typedef struct list_iterator
{
    list_node_t *curr;
    void *(*get)(struct list_iterator *);
    void (*next)(struct list_iterator *);
    int (*is_end)(struct list_iterator *);
    void (*del)(struct list_iterator *);
} list_iterator_t;

typedef struct list
{
    list_node_t *head;
    list_node_t *tail;
    void (*append)(struct list *list, void *data, void (*del)(void *));
    list_iterator_t *(*get_iter)(const struct list *list);
    void (*del)(struct list *list);
} list_t;

list_t *new_list();
#endif