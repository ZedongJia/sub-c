#include "debug.h"
#include <stdio.h>
#include <string.h>

void prettyTree(Node *node)
{
    int parr[256];
    for (int i = 0; i < 256; i++)
        parr[i] = 0;
    __prettyTree(node, parr, 0, 1);
}
void __prettyTree(Node *node, int *parr, int indent, int isLast)
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
    case KeywordExpression: {
        KeywordExpressionNode *keywordNode = (KeywordExpressionNode *)node->data;
        printf("%s:%s\n", getNodeTypeValue(node->nodeType), getTokenTypeValue(keywordNode->keywordType));
        break;
    }
    case LiteralExpression: {
        LiteralExpressionNode *literalNode = (LiteralExpressionNode *)node->data;
        printf("(%s)%s", getTokenTypeValue(literalNode->literalType), getNodeTypeValue(node->nodeType));
        printf(":%s\n", literalNode->value);
        break;
    }
    case BinaryOperatorExpression: {
        BinaryOperatorExpressionNode *binaryOperatorNode = (BinaryOperatorExpressionNode *)node->data;
        printf("%s:%s\n", getNodeTypeValue(node->nodeType), getTokenTypeValue(binaryOperatorNode->op));
        __prettyTree(binaryOperatorNode->left, parr, indent + 1, 0);
        __prettyTree(binaryOperatorNode->right, parr, indent + 1, 1);
        break;
    }
    case UnaryOperatorExpression: {
        UnaryOperatorExpressionNode *unaryOperatorNode = (UnaryOperatorExpressionNode *)node->data;
        printf("%s:%s\n", getNodeTypeValue(node->nodeType), getTokenTypeValue(unaryOperatorNode->op));
        __prettyTree(unaryOperatorNode->operand, parr, indent + 1, 1);
        break;
    }
    case Statement: {
        StatementNode *statementNode = (StatementNode *)node->data;
        printf("%s\n", getNodeTypeValue(node->nodeType));
        __prettyTree(statementNode->expression, parr, indent + 1, 1);
        break;
    }
    case Declaration: {
        DeclarationNode *declarationNode = (DeclarationNode *)node->data;
        printf("%s\n", getNodeTypeValue(node->nodeType));
        __prettyTree(declarationNode->type, parr, indent + 1, 0);
        __prettyTree(declarationNode->expression, parr, indent + 1, 1);
        break;
    }
    case Compound: {
        CompoundNode *compundNode = (CompoundNode *)node->data;
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
        ProgramNode *programNode = (ProgramNode *)node->data;
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
