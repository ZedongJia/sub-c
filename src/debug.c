#include "debug.h"
#include <stdio.h>
#include <string.h>

void prettyTree(Node *node)
{
    if (node == NULL)
        return;
    int parr[256];
    for (int i = 0; i < 256; i++)
        parr[i] = 0;
    __prettyTree(node, parr, 0, 1);
}

void __prettyNodeType(NodeType nodeType)
{
    printf("\033[32;1m%s\033[0m", getNodeTypeValue(nodeType));
}

void __prettyTokenType(TokenType tokenType)
{
    printf("\033[34;1m%s\033[0m", getTokenTypeValue(tokenType));
}

void __prettyTree(Node *node, int *parr, int indent, int isLast)
{
    for (int i = 0; i < indent; i++)
        printf(parr[i] ? "    " : "│   ");
    parr[indent] = isLast ? 1 : 0;
    printf(isLast ? "└── " : "├── ");
    __prettyNodeType(node->nodeType);
    printf(" ");
    switch (node->nodeType)
    {
    case Err: {
        printf("\n");
        break;
    }
    case TypeExpression: {
        TypeExpressionNode *keywordNode = (TypeExpressionNode *)node->data;
        __prettyTokenType(keywordNode->type);
        printf("\n");
        break;
    }
    case LiteralExpression: {
        LiteralExpressionNode *literalNode = (LiteralExpressionNode *)node->data;
        __prettyTokenType(literalNode->literalType);
        printf("(%s)\n", literalNode->value);
        break;
    }
    case BinaryOperatorExpression: {
        BinaryOperatorExpressionNode *binaryOperatorNode = (BinaryOperatorExpressionNode *)node->data;
        __prettyTokenType(binaryOperatorNode->op);
        printf("\n");
        __prettyTree(binaryOperatorNode->left, parr, indent + 1, 0);
        __prettyTree(binaryOperatorNode->right, parr, indent + 1, 1);
        break;
    }
    case UnaryOperatorExpression: {
        UnaryOperatorExpressionNode *unaryOperatorNode = (UnaryOperatorExpressionNode *)node->data;
        __prettyTokenType(unaryOperatorNode->op);
        printf("\n");
        __prettyTree(unaryOperatorNode->operand, parr, indent + 1, 1);
        break;
    }
    case Statement: {
        StatementNode *statementNode = (StatementNode *)node->data;
        printf("\n");
        __prettyTree(statementNode->expression, parr, indent + 1, 1);
        break;
    }
    case Declaration: {
        DeclarationNode *declarationNode = (DeclarationNode *)node->data;
        printf("\n");
        __prettyTree(declarationNode->type, parr, indent + 1, 0);
        __prettyTree(declarationNode->expression, parr, indent + 1, 1);
        break;
    }
    case Label: {
        LabelStatementNode *labelStatementNode = (LabelStatementNode *)node->data;
        printf("Label:%d\n", labelStatementNode->label);
        break;
    }
    case JumpIfFalse: {
        JumpIfFalseStatementNode *jumpIfFalseStatementNode = (JumpIfFalseStatementNode *)node->data;
        printf("Label:%d\n", jumpIfFalseStatementNode->label);
        __prettyTree(jumpIfFalseStatementNode->condition, parr, indent + 1, 1);
        break;
    case Jump: {
        JumpStatementNode *jumpStatementNode = (JumpStatementNode *)node->data;
        printf("Label:%d\n", jumpStatementNode->label);
        break;
    }
    case Scope: {
        ScopeNode *scopeNode = (ScopeNode *)node->data;
        printf("%s\n", scopeNode->inhert ? "Inhert" : "New");
        Node *p = scopeNode->head;
        while (p != NULL)
        {
            __prettyTree(p, parr, indent + 1, p->next == NULL);
            p = p->next;
        }
    }
    default:
        break;
    }
    }
}