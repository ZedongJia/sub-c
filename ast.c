#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void __ASTNode_del(void *node)
{
    struct ASTNode *_node = (struct ASTNode *)node;
    if (_node->ctype != NULL)
        free(_node->ctype);
    if (_node->value != NULL)
        free(_node->value);
    if (_node->children != NULL)
        _node->children->del(_node->children);
    if (_node->table != NULL)
        free(_node->table);
    free(node);
}

struct ASTNode *__ASTNode_create(Kind kind, const char *value)
{
    struct ASTNode *node = (struct ASTNode *)malloc(sizeof(struct ASTNode));
    // assign node type
    node->kind = kind;
    // copy value
    if (value != NULL)
    {
        node->value = (char *)malloc((strlen(value) + 1) * sizeof(char));
        strcpy(node->value, value);
    }
    else
    {
        node->value = NULL;
    }
    // set NULL
    node->ctype = NULL;
    node->children = NULL;
    node->table = NULL;
    node->prt = NULL;
    node->del = &__ASTNode_del;
    return node;
}

struct ASTNode *ASTNode_cliteral(struct CType *ctype, const char *value)
{
    struct ASTNode *node = __ASTNode_create(LIT_N, value);
    node->ctype = ctype;
    return node;
}

struct ASTNode *ASTNode_cunary(Kind kind, struct CType *ctype, struct ASTNode *operand)
{
    struct ASTNode *node = __ASTNode_create(kind, NULL);
    node->ctype = ctype;
    node->children = create_list();
    node->children->append(node->children, operand, operand->del);
    return node;
}

struct ASTNode *ASTNode_cbinary(Kind kind, struct CType *ctype, struct ASTNode *left, struct ASTNode *right)
{
    struct ASTNode *node = __ASTNode_create(kind, NULL);
    node->ctype = ctype;
    node->children = create_list();
    node->children->append(node->children, left, left->del);
    node->children->append(node->children, right, right->del);
    return node;
}

struct ASTNode *ASTNode_cdeclare(struct CType *ctype, const char *value, struct ASTNode *initializer)
{
    struct ASTNode *node = __ASTNode_create(DEC_N, value);
    node->ctype = ctype;
    if (initializer != NULL)
    {
        node->children = create_list();
        node->children->append(node->children, initializer, initializer->del);
    }
    return node;
}

struct ASTNode *ASTNode_clabel(int number)
{
    char buf[127];
    sprintf(buf, "label:%d", number);
    struct ASTNode *node = __ASTNode_create(LABEL_N, buf);
    return node;
}

struct ASTNode *ASTNode_cjump_false(struct ASTNode *condition, const char *value)
{
    struct ASTNode *node = __ASTNode_create(JUMP_FALSE_N, value);
    node->children = create_list();
    node->children->append(node->children, condition, condition->del);
    return node;
}

struct ASTNode *ASTNode_cjump(const char *value)
{
    struct ASTNode *node = __ASTNode_create(JUMP_N, value);
    return node;
}

struct ASTNode *ASTNode_cscope(struct ASTNode *prt)
{
    struct ASTNode *node = __ASTNode_create(SCOPE_N, NULL);
    node->prt = prt;
    node->table = (struct SymbolTable *)malloc(sizeof(struct SymbolTable));
    node->table->num_var = 0;
    node->children = create_list();
    return node;
}
