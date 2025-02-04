#include "utils.h"
#include <stdio.h>
#include <string.h>

void __prettyPrefix(int *parr, int indent, int isLast, const char *end)
{
    printf("\n");
    for (int i = 0; i < indent; i++)
        printf(parr[i] ? "    " : "│   ");
    printf(isLast ? "└─%s " : "├─%s ", end);
}

void __prettyASTNodeType(Kind kind)
{
    printf("\033[33;1m%s\033[0m", kind_name(kind));
}

void __prettyPropertyName(const char *name)
{
    printf("\033[36;1m%s\033[0m", name);
}

void __debug_print_CType(const struct CType *ctype)
{
    if (!ctype->mod)
        printf("const ");
    printf("%s", type_name(ctype->type));
    if (ctype->ptr)
    {
        for (int i = 0; i < ctype->ptr; i++)
            printf("*");
    }
}

void __prettyTree(struct ASTNode *node, int *parr, int indent, int isLast)
{
    __prettyPrefix(parr, indent, isLast, "─");
    parr[indent] = isLast ? 1 : 0;
    __prettyASTNodeType(node->kind);
    // type
    if (node->ctype != NULL)
    {
        __prettyPrefix(parr, indent + 1, node->value == NULL && node->children == NULL, "*");
        __prettyPropertyName("Type: ");
        __debug_print_CType(node->ctype);
        printf("(%dbytes)", node->ctype->offset[node->ctype->ptr]);
    }
    // value
    if (node->value != NULL)
    {
        __prettyPrefix(parr, indent + 1, node->children == NULL, "*");
        __prettyPropertyName("Value: ");
        printf("%s", node->value);
    }
    // children
    if (node->children != NULL)
    {
        struct ListIterator *iter = create_list_iterator(node->children);
        while (!iter->end(iter))
        {
            void *data = iter->data(iter);
            iter->next(iter);
            __prettyTree((struct ASTNode *)data, parr, indent + 1, iter->end(iter));
        }
    }
}

void __debug_pretty_tree(struct ASTNode *node)
{
    if (node == NULL)
        return;
    int parr[256];
    for (int i = 0; i < 256; i++)
        parr[i] = 0;

    printf("[Tree]");
    __prettyTree(node, parr, 0, 1);
    printf("\n");
}