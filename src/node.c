#include "node.h"
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
    case Compound:
        return "Compound";
    case Program:
        return "Program";
    default:
        return "UnexpectedNode";
    }
}
Node *createNode(NodeType nodeType, void *data)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->nodeType = nodeType;
    node->data = data;
    return node;
}
void freeNode(Node *node)
{
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
        break;
    case Declaration:
        freeDeclarationNode((DeclarationNode *)node->data);
        break;
    case Compound:
        freeCompoundNode((CompoundNode *)node->data);
        break;
    case Program:
        freeProgramNode((ProgramNode *)node->data);
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
Node *createStatementNode(Node *expression, TokenType delimiter)
{
    StatementNode *statementNode = (StatementNode *)malloc(sizeof(StatementNode));
    statementNode->expression = expression;
    statementNode->delimiter = delimiter;
    return createNode(Statement, (void *)statementNode);
}
void freeStatementNode(StatementNode *node)
{
    if (node->expression != NULL)
        freeNode(node->expression);
    node->expression = NULL;
}
Node *createDeclarationNode(Node *type, Node *expression, TokenType delimiter)
{
    DeclarationNode *declarationNode = (DeclarationNode *)malloc(sizeof(DeclarationNode));
    declarationNode->type = type;
    declarationNode->expression = expression;
    declarationNode->delimiter = delimiter;
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
Node *createCompoundNode(TokenType openDelimiter, Node **statements, int size, TokenType closeDelimiter)
{
    CompoundNode *compoundNode = (CompoundNode *)malloc(sizeof(CompoundNode));
    compoundNode->openDelimiter = openDelimiter;
    compoundNode->statements = statements;
    compoundNode->size = size;
    compoundNode->closeDelimiter = closeDelimiter;
    return createNode(Compound, (void *)compoundNode);
}
void freeCompoundNode(CompoundNode *node)
{
    if (node->statements != NULL)
    {
        for (int i = 0; i < node->size; i++)
        {
            freeNode(node->statements[i]);
            node->statements[i] = NULL;
        }
        free(node->statements);
    }
    node->statements = NULL;
}
Node *createProgramNode(Node **statements, int size, TokenType endDelimiter)
{
    ProgramNode *programNode = (ProgramNode *)malloc(sizeof(ProgramNode));
    programNode->statements = statements;
    programNode->size = size;
    programNode->endDelimiter = endDelimiter;
    return createNode(Program, (void *)programNode);
}
void freeProgramNode(ProgramNode *node)
{
    if (node->statements != NULL)
    {
        for (int i = 0; i < node->size; i++)
        {
            freeNode(node->statements[i]);
            node->statements[i] = NULL;
        }
        free(node->statements);
    }
    node->statements = NULL;
}
