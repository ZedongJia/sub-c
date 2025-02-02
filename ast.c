#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode *__createASTNode(Kind kind, const char *value)
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
    node->baseType = NULL;
    node->children = NULL;
    node->table = NULL;
    node->parent = NULL;
    return node;
}

void freeASTNode(void *node)
{
    ASTNode *_node = (ASTNode *)node;
    if (_node->baseType != NULL)
        free(_node->baseType);
    if (_node->value != NULL)
        free(_node->value);
    if (_node->children != NULL)
        freeList(_node->children, freeASTNode);
    if (_node->table != NULL)
        free(_node->table);
    free(node);
}

ASTNode *cLiteral(BaseType *baseType, char *value)
{
    ASTNode *node = __createASTNode(LIT_N, value);
    node->baseType = baseType;
    return node;
}

ASTNode *cUnary(Kind kind, BaseType *baseType, ASTNode *operand)
{
    ASTNode *node = __createASTNode(kind, NULL);
    node->baseType = baseType;
    node->children = createList();
    appendToList(node->children, (void *)operand);
    return node;
}

ASTNode *cBinary(Kind kind, BaseType *baseType, ASTNode *left, ASTNode *right)
{
    ASTNode *node = __createASTNode(kind, NULL);
    node->baseType = baseType;
    node->children = createList();
    appendToList(node->children, (void *)left);
    appendToList(node->children, (void *)right);
    return node;
}

ASTNode *cDeclare(BaseType *baseType, char *value)
{
    ASTNode *node = __createASTNode(DEC_N, value);
    node->baseType = baseType;
    return node;
}

ASTNode *cLabel(int number)
{
    char buf[127];
    sprintf(buf, "label:%d", number);
    ASTNode *node = __createASTNode(LABEL_N, buf);
    return node;
}

ASTNode *cJumpFalse(ASTNode *condition, char *value)
{
    ASTNode *node = __createASTNode(JUMP_FALSE_N, value);
    node->children = createList();
    appendToList(node->children, (void *)condition);
    return node;
}

ASTNode *cJump(char *value)
{
    ASTNode *node = __createASTNode(JUMP_N, value);
    return node;
}

ASTNode *cScope(ASTNode *parent)
{
    ASTNode *node = __createASTNode(SCOPE_N, NULL);
    node->parent = parent;
    node->table = (SymbolTable *)malloc(sizeof(SymbolTable));
    node->table->num_var = 0;
    node->children = createList();
    return node;
}
