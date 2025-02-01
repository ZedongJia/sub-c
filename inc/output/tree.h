#ifndef TREE
#define TREE
#include "syntax/node.h"
void prettyTree(Node *node);
void __prettyPrefix(int *parr, int indent, int isLast, const char *end);
void __prettyNodeType(NodeType nodeType);
void __prettyTokenType(TokenType type);
void __prettyPropertyName(const char *name);
void __prettyBaseType(BaseType *baseType);
void __prettyTree(Node *node, int *parr, int indent, int isLast);
#endif