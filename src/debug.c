#include "debug.h"
#include <stdio.h>
#include <string.h>

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
    case LiteralExpression: {
        struct LiteralExpressionNode *literalNode = (struct LiteralExpressionNode *)node->data;
        printf("(%s)%s:%s\n", getTokenTypeValue(literalNode->literalType), getNodeTypeValue(node->nodeType),
               literalNode->value);
        break;
    }
    case BinaryOperatorExpression: {
        struct BinaryOperatorExpressionNode *binaryOperatorNode = (struct BinaryOperatorExpressionNode *)node->data;
        printf("%s:%s\n", getNodeTypeValue(node->nodeType), getTokenTypeValue(binaryOperatorNode->op));
        __prettyTree(binaryOperatorNode->left, parr, indent + 1, 0);
        __prettyTree(binaryOperatorNode->right, parr, indent + 1, 1);
        break;
    }
    case UnaryOperatorExpression: {
        struct UnaryOperatorExpressionNode *unaryOperatorNode = (struct UnaryOperatorExpressionNode *)node->data;
        printf("%s:%s\n", getNodeTypeValue(node->nodeType), getTokenTypeValue(unaryOperatorNode->op));
        __prettyTree(unaryOperatorNode->operand, parr, indent + 1, 1);
        break;
    }
    case Statement: {
        struct StatementNode *statementNode = (struct StatementNode *)node->data;
        printf("%s\n", getNodeTypeValue(node->nodeType));
        __prettyTree(statementNode->expression, parr, indent + 1, 1);
        break;
    }
    case Compound: {
        struct CompoundNode *compundNode = (struct CompoundNode *)node->data;
        printf("%s\n", getNodeTypeValue(node->nodeType));
        if (compundNode->statements != NULL)
        {
            for (int i = 0; i < compundNode->size - 1; i++)
                __prettyTree(compundNode->statements[i], parr, indent + 1, 0);
            __prettyTree(compundNode->statements[compundNode->size - 1], parr, indent + 1, 1);
        }
        break;
    }
    case Program: {
        struct ProgramNode *programNode = (struct ProgramNode *)node->data;
        printf("%s\n", getNodeTypeValue(node->nodeType));
        if (programNode->statements != NULL)
        {
            for (int i = 0; i < programNode->size - 1; i++)
                __prettyTree(programNode->statements[i], parr, indent + 1, 0);
            __prettyTree(programNode->statements[programNode->size - 1], parr, indent + 1, 1);
        }
        break;
    }
    default:
        break;
    }
}
