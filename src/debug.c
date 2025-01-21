#include "debug.h"
#include <stdio.h>
#include <string.h>
/**
 *  "├── ","└── ","│   "
 *
 */

void prettyTree(struct Node *node)
{
    int parr[256];
    for (int i = 0; i < 256; i++)
        parr[i] = 0;
    __prettyTree(node, parr, 0, 1);
}

void __prettyTree(struct Node *node, int *parr, int indent, int isLast)
{
    for (int i = 0; i < indent; i++)
        printf(parr[i] ? "    " : "│   ");
    parr[indent] = isLast ? 1 : 0;
    printf(isLast ? "└── " : "├── ");
    switch (node->nodeType)
    {
    case Err: {
        printf("%s\n", getNodeTypeValue(node->nodeType));
        break;
    }
    case Literal: {
        struct LiteralNode *literalNode = (struct LiteralNode *)node->data;
        printf("%s:%s\n", getNodeTypeValue(node->nodeType), literalNode->value);
        break;
    }
    case BinaryOperator: {
        struct BinaryOperatorNode *binaryOperatorNode = (struct BinaryOperatorNode *)node->data;
        printf("%s:%s\n", getNodeTypeValue(node->nodeType), getTokenTypeValue(binaryOperatorNode->op));
        __prettyTree(binaryOperatorNode->left, parr, indent + 1, 0);
        __prettyTree(binaryOperatorNode->right, parr, indent + 1, 1);
        break;
    }
    case UnaryOperator: {
        struct UnaryOperatorNode *unaryOperatorNode = (struct UnaryOperatorNode *)node->data;
        printf("%s:%s\n", getNodeTypeValue(node->nodeType), getTokenTypeValue(unaryOperatorNode->op));
        __prettyTree(unaryOperatorNode->operand, parr, indent + 1, 1);
        break;
    }
    default:
        break;
    }
}
