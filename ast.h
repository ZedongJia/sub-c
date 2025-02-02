#ifndef AST
#define AST
#include "list.h"
#include "node.h"
#include "symbol.h"
#include "types.h"

typedef struct ASTNode
{
    Kind kind;      // ast node kind
    CType *ctype;   // ast value type (only expression valid)
    char *value;    // ast value
    List *children; // ast children

    SymbolTable *table;     // symbol
    struct ASTNode *parent; // scope chain
} ASTNode;

void ASTNode_del(void *node);
ASTNode *ASTNode_cLiteral(CType *ctype, char *value);
ASTNode *ASTNode_cUnary(Kind kind, CType *ctype, ASTNode *operand);
ASTNode *ASTNode_cBinary(Kind kind, CType *ctype, ASTNode *left, ASTNode *right);
ASTNode *ASTNode_cDeclare(CType *ctype, char *value, ASTNode *initializer);
ASTNode *ASTNode_cLabel(int number);
ASTNode *ASTNode_cJumpFalse(ASTNode *condition, char *value);
ASTNode *ASTNode_cJump(char *value);
ASTNode *ASTNode_cScope(ASTNode *parent);
#endif