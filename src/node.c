#include "node.h"
#include <stdlib.h>
#include <string.h>

char *getNodeTypeValue(enum NodeType nodeType)
{
    switch (nodeType)
    {
    case Err:
        return "Err";
    case Literal:
        return "Literal";
    case BinaryOperator:
        return "BinaryOperator";
    case UnaryOperator:
        return "UnaryOperator";
    default:
        return "UnexpectedNode";
    }
}
struct Node *createNode(enum NodeType nodeType, void *data)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    node->nodeType = nodeType;
    node->data = data;
    return node;
}
struct Node *createLiteralNode(char *value, int valueLength)
{
    struct LiteralNode *literalNode = (struct LiteralNode *)malloc(sizeof(struct LiteralNode));
    literalNode->value = (char *)malloc(valueLength * sizeof(char));
    strcpy(literalNode->value, value);
    return createNode(Literal, (void *)literalNode);
}
struct Node *createBinaryOperatorNode(struct Node *left, enum TokenType op, struct Node *right)
{
    struct BinaryOperatorNode *binaryOperatorNode =
        (struct BinaryOperatorNode *)malloc(sizeof(struct BinaryOperatorNode));
    binaryOperatorNode->left = left;
    binaryOperatorNode->op = op;
    binaryOperatorNode->right = right;
    return createNode(BinaryOperator, (void *)binaryOperatorNode);
}
struct Node *createUnaryOperatorNode(enum TokenType op, struct Node *operand)
{
    struct UnaryOperatorNode *unaryOperatorNode = (struct UnaryOperatorNode *)malloc(sizeof(struct UnaryOperatorNode));
    unaryOperatorNode->op = op;
    unaryOperatorNode->operand = operand;
    return createNode(UnaryOperator, (void *)unaryOperatorNode);
}
void freeNode(struct Node *node)
{
    switch (node->nodeType)
    {
    case Literal:
        freeLiteralNode((struct LiteralNode *)node->data);
        free(node->data);
        break;
    case BinaryOperator:
        freeBinaryOperatorNode((struct BinaryOperatorNode *)node->data);
        free(node->data);
        break;
    default:
        break;
    }
    node->data = NULL;
    free(node);
}
void freeLiteralNode(struct LiteralNode *node)
{
    free(node->value);
    node->value = NULL;
}
void freeBinaryOperatorNode(struct BinaryOperatorNode *node)
{
    freeNode(node->left);
    node->left = NULL;
    freeNode(node->right);
    node->right = NULL;
}