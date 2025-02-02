#include "tree.h"
#include <stdio.h>
#include <string.h>

void prettyTree(ASTNode *node)
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

void __prettyPrefix(int *parr, int indent, int isLast, const char *end)
{
    printf("\n");
    for (int i = 0; i < indent; i++)
        printf(parr[i] ? "    " : "│   ");
    printf(isLast ? "└─%s " : "├─%s ", end);
}

void __prettyASTNodeType(Kind kind)
{
    printf("\033[33;1m%s\033[0m", kindName(kind));
}

void __prettyPropertyName(const char *name)
{
    printf("\033[36;1m%s\033[0m", name);
}

void __prettyCType(const CType *ctype)
{
    if (!ctype->modify)
        printf("const ");
    printf("%s", typeName(ctype->type));
    if (ctype->ptr)
    {
        for (int i = 0; i < ctype->ptr; i++)
            printf("*");
    }
}

void __prettyTree(ASTNode *node, int *parr, int indent, int isLast)
{
    __prettyPrefix(parr, indent, isLast, "─");
    parr[indent] = isLast ? 1 : 0;
    __prettyASTNodeType(node->kind);
    // type
    if (node->ctype != NULL)
    {
        __prettyPrefix(parr, indent + 1, node->value == NULL && node->children == NULL, "*");
        __prettyPropertyName("Type: ");
        __prettyCType(node->ctype);
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
        ListNode *curr = node->children->head;
        while (curr != NULL)
        {
            __prettyTree((ASTNode *)curr->data, parr, indent + 1, curr->next == NULL);
            curr = curr->next;
        }
    }
}