#ifndef DEBUG
#define DEBUG
#include "node.h"
void prettyTree(struct Node *node);
void __prettyTree(struct Node *node, int *parr, int indent, int isLast);
#endif