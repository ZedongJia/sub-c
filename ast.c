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

struct ASTNode *__ASTNode_new(Kind kind, const char *value)
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

struct ASTNode *new_literal(struct CType *ctype, const char *value)
{
    struct ASTNode *node = __ASTNode_new(LIT_N, value);
    node->ctype = ctype;
    return node;
}

struct ASTNode *new_unary(Kind kind, struct CType *ctype, struct ASTNode *operand)
{
    struct ASTNode *node = __ASTNode_new(kind, NULL);
    node->ctype = ctype;
    node->children = new_list();
    node->children->append(node->children, operand, operand->del);
    return node;
}

struct ASTNode *new_binary(Kind kind, struct CType *ctype, struct ASTNode *left, struct ASTNode *right)
{
    struct ASTNode *node = __ASTNode_new(kind, NULL);
    node->ctype = ctype;
    node->children = new_list();
    node->children->append(node->children, left, left->del);
    node->children->append(node->children, right, right->del);
    return node;
}

struct ASTNode *new_declare(struct CType *ctype, const char *value, struct ASTNode *initializer)
{
    struct ASTNode *node = __ASTNode_new(DEC_N, value);
    node->ctype = ctype;
    if (initializer != NULL)
    {
        node->children = new_list();
        node->children->append(node->children, initializer, initializer->del);
    }
    return node;
}

struct ASTNode *new_label(int number)
{
    char buf[127];
    sprintf(buf, "label:%d", number);
    struct ASTNode *node = __ASTNode_new(LABEL_N, buf);
    return node;
}

struct ASTNode *new_jump_false(struct ASTNode *condition, const char *value)
{
    struct ASTNode *node = __ASTNode_new(JUMP_FALSE_N, value);
    node->children = new_list();
    node->children->append(node->children, condition, condition->del);
    return node;
}

struct ASTNode *new_jump(const char *value)
{
    struct ASTNode *node = __ASTNode_new(JUMP_N, value);
    return node;
}

struct ASTNode *new_scope(struct ASTNode *prt)
{
    struct ASTNode *node = __ASTNode_new(SCOPE_N, NULL);
    node->prt = prt;
    node->table = (struct SymbolTable *)malloc(sizeof(struct SymbolTable));
    node->table->num_var = 0;
    node->children = new_list();
    return node;
}
