#include "node.h"
#include <stdlib.h>
#include <string.h>

char *getNodeTypeValue(enum NodeType nodeType)
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
struct Node *createNode(enum NodeType nodeType, void *data)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    node->nodeType = nodeType;
    node->data = data;
    return node;
}
void freeNode(struct Node *node)
{
    switch (node->nodeType)
    {
    case KeywordExpression:
        freeKeywordExpressionNode((struct KeywordExpressionNode *)node->data);
        break;
    case LiteralExpression:
        freeLiteralExpressionNode((struct LiteralExpressionNode *)node->data);
        break;
    case BinaryOperatorExpression:
        freeBinaryOperatorExpressionNode((struct BinaryOperatorExpressionNode *)node->data);
        break;
    case UnaryOperatorExpression:
        freeUnaryOperatorExpressionNode((struct UnaryOperatorExpressionNode *)node->data);
        break;
    case Statement:
        freeStatementNode((struct StatementNode *)node->data);
        break;
    case Declaration:
        freeDeclarationNode((struct DeclarationNode *)node->data);
        break;
    case Compound:
        freeCompoundNode((struct CompoundNode *)node->data);
        break;
    case Program:
        freeProgramNode((struct ProgramNode *)node->data);
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
struct Node *createKeywordExpressionNode(enum TokenType keywordType)
{
    struct KeywordExpressionNode *keywordNode =
        (struct KeywordExpressionNode *)malloc(sizeof(struct KeywordExpressionNode));
    keywordNode->keywordType = keywordType;
    return createNode(KeywordExpression, (void *)keywordNode);
}
void freeKeywordExpressionNode(struct KeywordExpressionNode *node)
{
    return;
}
struct Node *createLiteralExpressionNode(enum TokenType literalType, char *value, int length)
{
    struct LiteralExpressionNode *literalNode =
        (struct LiteralExpressionNode *)malloc(sizeof(struct LiteralExpressionNode));
    literalNode->literalType = literalType;
    literalNode->value = (char *)malloc(length * sizeof(char));
    strcpy(literalNode->value, value);
    literalNode->length = length;
    return createNode(LiteralExpression, (void *)literalNode);
}
void freeLiteralExpressionNode(struct LiteralExpressionNode *node)
{
    if (node->value != NULL)
        free(node->value);
    node->value = NULL;
}
struct Node *createBinaryOperatorExpressionNode(struct Node *left, enum TokenType op, struct Node *right)
{
    struct BinaryOperatorExpressionNode *binaryOperatorNode =
        (struct BinaryOperatorExpressionNode *)malloc(sizeof(struct BinaryOperatorExpressionNode));
    binaryOperatorNode->left = left;
    binaryOperatorNode->op = op;
    binaryOperatorNode->right = right;
    return createNode(BinaryOperatorExpression, (void *)binaryOperatorNode);
}
void freeBinaryOperatorExpressionNode(struct BinaryOperatorExpressionNode *node)
{
    if (node->left != NULL)
        freeNode(node->left);
    node->left = NULL;
    if (node->right != NULL)
        freeNode(node->right);
    node->right = NULL;
}
struct Node *createUnaryOperatorExpressionNode(enum TokenType op, struct Node *operand)
{
    struct UnaryOperatorExpressionNode *unaryOperatorNode =
        (struct UnaryOperatorExpressionNode *)malloc(sizeof(struct UnaryOperatorExpressionNode));
    unaryOperatorNode->op = op;
    unaryOperatorNode->operand = operand;
    return createNode(UnaryOperatorExpression, (void *)unaryOperatorNode);
}
void freeUnaryOperatorExpressionNode(struct UnaryOperatorExpressionNode *node)
{
    if (node->operand != NULL)
        freeNode(node->operand);
    node->operand = NULL;
}
struct Node *createStatementNode(struct Node *expression, enum TokenType delimiter)
{
    struct StatementNode *statementNode = (struct StatementNode *)malloc(sizeof(struct StatementNode));
    statementNode->expression = expression;
    statementNode->delimiter = delimiter;
    return createNode(Statement, (void *)statementNode);
}
void freeStatementNode(struct StatementNode *node)
{
    if (node->expression != NULL)
        freeNode(node->expression);
    node->expression = NULL;
}
struct Node *createDeclarationNode(struct Node *type, struct Node *expression, enum TokenType delimiter)
{
    struct DeclarationNode *declarationNode = (struct DeclarationNode *)malloc(sizeof(struct DeclarationNode));
    declarationNode->type = type;
    declarationNode->expression = expression;
    declarationNode->delimiter = delimiter;
    return createNode(Declaration, (void *)declarationNode);
}
void freeDeclarationNode(struct DeclarationNode *node)
{
    if (node->type != NULL)
        freeNode(node->type);
    node->type = NULL;
    if (node->expression != NULL)
        freeNode(node->expression);
    node->expression = NULL;
}
struct Node *createCompoundNode(struct Node **statements, int size)
{
    struct CompoundNode *compoundNode = (struct CompoundNode *)malloc(sizeof(struct CompoundNode));
    compoundNode->statements = statements;
    compoundNode->size = size;
    return createNode(Compound, (void *)compoundNode);
}
void freeCompoundNode(struct CompoundNode *node)
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
struct Node *createProgramNode(struct Node **statements, int size)
{
    struct ProgramNode *programNode = (struct ProgramNode *)malloc(sizeof(struct ProgramNode));
    programNode->statements = statements;
    programNode->size = size;
    return createNode(Program, (void *)programNode);
}
void freeProgramNode(struct ProgramNode *node)
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
