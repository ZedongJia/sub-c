#ifndef AST
#define AST
#include "list.h"
#include "node.h"
#include "symbol.h"
#include "types.h"

typedef struct ASTNode
{
    Kind kind;          // ast node kind
    BaseType *baseType; // ast value type (only expression valid)
    char *value;        // ast value
    List *children;     // ast children

    SymbolTable *table;     // symbol
    struct ASTNode *parent; // scope chain
} ASTNode;

void freeASTNode(void *node);
ASTNode *cLiteral(BaseType *baseType, char *value);
ASTNode *cUnary(Kind kind, BaseType *baseType, ASTNode *operand);
ASTNode *cBinary(Kind kind, BaseType *baseType, ASTNode *left, ASTNode *right);
ASTNode *cDeclare(BaseType *baseType, char *value);
ASTNode *cLabel(int number);
ASTNode *cJumpFalse(ASTNode *condition, char *value);
ASTNode *cJump(char *value);
ASTNode *cScope(ASTNode *parent);
#endif