#include "ast.h"
#ifndef TREE
#define TREE
void prettyTree(ASTNode *ASTNode);
void __prettyPrefix(int *parr, int indent, int isLast, const char *end);
void __prettyASTNodeType(Kind kind);
void __prettyTokenType(Token type);
void __prettyPropertyName(const char *name);
void __prettyBaseType(BaseType *baseType);
void __prettyTree(ASTNode *ASTNode, int *parr, int indent, int isLast);
#endif