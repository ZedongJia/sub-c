#include "syntax/node.h"
#include <stdlib.h>
#include <string.h>

char *getNodeTypeValue(NodeType nodeType)
{
    switch (nodeType)
    {
    case Err:
        return "Err";
    case KeywordExpression:
        return "KeywordExpression";
    case LiteralExpression:
        return "LiteralExpression";
    case BinaryOperatorExpression:
        return "BinaryOperatorExpression";
    case UnaryOperatorExpression:
        return "UnaryOperatorExpression";
    case Statement:
        return "Statement";
    case Declaration:
        return "Declaration";
    default:
        return "UnexpectedNode";
    }
}
Node *createNode(NodeType nodeType, void *data)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->nodeType = nodeType;
    node->data = data;
    node->next = NULL;
    return node;
}
void freeNode(Node *node)
{
    if (node->next != NULL)
    {
        freeNode(node->next);
        node->next = NULL;
    }
    switch (node->nodeType)
    {
    case KeywordExpression:
        freeKeywordExpressionNode((KeywordExpressionNode *)node->data);
        break;
    case LiteralExpression:
        freeLiteralExpressionNode((LiteralExpressionNode *)node->data);
        break;
    case BinaryOperatorExpression:
        freeBinaryOperatorExpressionNode((BinaryOperatorExpressionNode *)node->data);
        break;
    case UnaryOperatorExpression:
        freeUnaryOperatorExpressionNode((UnaryOperatorExpressionNode *)node->data);
        break;
    case Statement:
        freeStatementNode((StatementNode *)node->data);
    case Declaration:
        freeDeclarationNode((DeclarationNode *)node->data);
        break;
    default:
        break;
    }
    if (node->data != NULL)
    {
        free(node->data);
        node->data = NULL;
    }
    free(node);
}
Node *createKeywordExpressionNode(TokenType keywordType)
{
    KeywordExpressionNode *keywordNode = (KeywordExpressionNode *)malloc(sizeof(KeywordExpressionNode));
    keywordNode->keywordType = keywordType;
    return createNode(KeywordExpression, (void *)keywordNode);
}
void freeKeywordExpressionNode(KeywordExpressionNode *node)
{
    return;
}
Node *createLiteralExpressionNode(TokenType literalType, char *value, int length)
{
    LiteralExpressionNode *literalNode = (LiteralExpressionNode *)malloc(sizeof(LiteralExpressionNode));
    literalNode->literalType = literalType;
    literalNode->value = (char *)malloc(length * sizeof(char));
    strcpy(literalNode->value, value);
    literalNode->length = length;
    return createNode(LiteralExpression, (void *)literalNode);
}
void freeLiteralExpressionNode(LiteralExpressionNode *node)
{
    if (node->value != NULL)
        free(node->value);
    node->value = NULL;
}
Node *createUnaryOperatorExpressionNode(TokenType op, Node *operand)
{
    UnaryOperatorExpressionNode *unaryOperatorNode =
        (UnaryOperatorExpressionNode *)malloc(sizeof(UnaryOperatorExpressionNode));
    unaryOperatorNode->op = op;
    unaryOperatorNode->operand = operand;
    return createNode(UnaryOperatorExpression, (void *)unaryOperatorNode);
}
void freeUnaryOperatorExpressionNode(UnaryOperatorExpressionNode *node)
{
    if (node->operand != NULL)
        freeNode(node->operand);
    node->operand = NULL;
}
Node *createBinaryOperatorExpressionNode(Node *left, TokenType op, Node *right)
{
    BinaryOperatorExpressionNode *binaryOperatorNode =
        (BinaryOperatorExpressionNode *)malloc(sizeof(BinaryOperatorExpressionNode));
    binaryOperatorNode->left = left;
    binaryOperatorNode->op = op;
    binaryOperatorNode->right = right;
    return createNode(BinaryOperatorExpression, (void *)binaryOperatorNode);
}
void freeBinaryOperatorExpressionNode(BinaryOperatorExpressionNode *node)
{
    if (node->left != NULL)
        freeNode(node->left);
    node->left = NULL;
    if (node->right != NULL)
        freeNode(node->right);
    node->right = NULL;
}
Node *createStatementNode(Node *expression)
{
    StatementNode *statementNode = (StatementNode *)malloc(sizeof(StatementNode));
    statementNode->expression = expression;
    return createNode(Statement, (void *)statementNode);
}
void freeStatementNode(StatementNode *node)
{
    if (node->expression != NULL)
        freeNode(node->expression);
    node->expression = NULL;
}
Node *createDeclarationNode(Node *type, Node *expression)
{
    DeclarationNode *declarationNode = (DeclarationNode *)malloc(sizeof(DeclarationNode));
    declarationNode->type = type;
    declarationNode->expression = expression;
    return createNode(Declaration, (void *)declarationNode);
}
void freeDeclarationNode(DeclarationNode *node)
{
    if (node->type != NULL)
        freeNode(node->type);
    node->type = NULL;
    if (node->expression != NULL)
        freeNode(node->expression);
    node->expression = NULL;
}