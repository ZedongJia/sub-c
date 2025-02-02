#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode *__ASTNode_create(Kind kind, const char *value)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
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
    node->parent = NULL;
    return node;
}

void ASTNode_del(void *node)
{
    ASTNode *_node = (ASTNode *)node;
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

ASTNode *ASTNode_cLiteral(CType *ctype, char *value)
{
    ASTNode *node = __ASTNode_create(LIT_N, value);
    node->ctype = ctype;
    return node;
}

ASTNode *ASTNode_cUnary(Kind kind, CType *ctype, ASTNode *operand)
{
    ASTNode *node = __ASTNode_create(kind, NULL);
    node->ctype = ctype;
    node->children = createList(ASTNode_del);
    node->children->append(node->children, operand);
    return node;
}

ASTNode *ASTNode_cBinary(Kind kind, CType *ctype, ASTNode *left, ASTNode *right)
{
    ASTNode *node = __ASTNode_create(kind, NULL);
    node->ctype = ctype;
    node->children = createList(ASTNode_del);
    node->children->append(node->children, left);
    node->children->append(node->children, right);
    return node;
}

ASTNode *ASTNode_cDeclare(CType *ctype, char *value, ASTNode *initializer)
{
    ASTNode *node = __ASTNode_create(DEC_N, value);
    node->ctype = ctype;
    if (initializer != NULL)
    {
        node->children = createList(ASTNode_del);
        node->children->append(node->children, initializer);
    }
    return node;
}

ASTNode *ASTNode_cLabel(int number)
{
    char buf[127];
    sprintf(buf, "label:%d", number);
    ASTNode *node = __ASTNode_create(LABEL_N, buf);
    return node;
}

ASTNode *ASTNode_cJumpFalse(ASTNode *condition, char *value)
{
    ASTNode *node = __ASTNode_create(JUMP_FALSE_N, value);
    node->children = createList(ASTNode_del);
    node->children->append(node->children, condition);
    return node;
}

ASTNode *ASTNode_cJump(char *value)
{
    ASTNode *node = __ASTNode_create(JUMP_N, value);
    return node;
}

ASTNode *ASTNode_cScope(ASTNode *parent)
{
    ASTNode *node = __ASTNode_create(SCOPE_N, NULL);
    node->parent = parent;
    node->table = (SymbolTable *)malloc(sizeof(SymbolTable));
    node->table->num_var = 0;
    node->children = createList(ASTNode_del);
    return node;
}
