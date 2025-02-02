#ifndef TREE
#define TREE
#include "ast.h"
void prettyTree(ASTNode *ASTNode);
void __prettyPrefix(int *parr, int indent, int isLast, const char *end);
void __prettyASTNodeType(Kind kind);
void __prettyPropertyName(const char *name);
void __prettyCType(const CType *ctype);
void __prettyTree(ASTNode *ASTNode, int *parr, int indent, int isLast);
#endif