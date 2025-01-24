#ifndef DEBUG
#define DEBUG
#include "syntax/node.h"
void prettyTree(Node *node);
void __prettyTree(Node *node, int *parr, int indent, int isLast);
#endif