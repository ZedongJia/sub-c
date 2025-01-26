#include "syntax/node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getNodeTypeValue(NodeType nodeType)
{
    switch (nodeType)
    {
    case Err:
        return "Err";
    case TypeExpression:
        return "Type";
    case LiteralExpression:
        return "Literal";
    case BinaryOperatorExpression:
        return "BinaryOperator";
    case UnaryOperatorExpression:
        return "UnaryOperator";
    case Statement:
        return "Statement";
    case Declaration:
        return "Declaration";
    case Label:
        return "Label";
    case JumpIfFalse:
        return "JumpIfFalse";
    case Jump:
        return "Jump";
    case Scope:
        return "Scope";
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
    node->prev = NULL;
    return node;
}

void freeNode(Node *node)
{
    // free data
    switch (node->nodeType)
    {
    case TypeExpression:
        freeTypeExpressionNode((TypeExpressionNode *)node->data);
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
    case Label:
        freeLabelStatementNode((LabelStatementNode *)node->data);
        break;
    case JumpIfFalse:
        freeJumpIfFalseStatementNode((JumpIfFalseStatementNode *)node->data);
        break;
    case Jump:
        freeJumpStatementNode((JumpStatementNode *)node->data);
        break;
    case Scope:
        freeScopeNode((ScopeNode *)node->data);
        break;
    default:
        break;
    }
    node->data = NULL;
    free(node);
}

Node *createTypeExpressionNode(TokenType type)
{
    TypeExpressionNode *typeNode = (TypeExpressionNode *)malloc(sizeof(TypeExpressionNode));
    typeNode->type = type;
    return createNode(TypeExpression, (void *)typeNode);
}

void freeTypeExpressionNode(TypeExpressionNode *node)
{
    free(node);
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
    free(node);
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
    freeNode(node->operand);
    node->operand = NULL;
    free(node);
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
    freeNode(node->left);
    node->left = NULL;
    freeNode(node->right);
    node->right = NULL;
    free(node);
}

Node *createStatementNode(Node *expression)
{
    StatementNode *statementNode = (StatementNode *)malloc(sizeof(StatementNode));
    statementNode->expression = expression;
    return createNode(Statement, (void *)statementNode);
}

void freeStatementNode(StatementNode *node)
{
    freeNode(node->expression);
    node->expression = NULL;
    free(node);
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
    freeNode(node->type);
    node->type = NULL;
    freeNode(node->expression);
    node->expression = NULL;
    free(node);
}

Node *createLabelStatementNode(int label)
{
    LabelStatementNode *labelStatementNode = (LabelStatementNode *)malloc(sizeof(LabelStatementNode));
    labelStatementNode->label = label;
    return createNode(Label, (void *)labelStatementNode);
}

void freeLabelStatementNode(LabelStatementNode *node)
{
    free(node);
}

Node *createJumpIfFalseStatementNode(Node *condition, int label)
{
    JumpIfFalseStatementNode *jumpIfFalseStatementNode =
        (JumpIfFalseStatementNode *)malloc(sizeof(JumpIfFalseStatementNode));
    jumpIfFalseStatementNode->condition = condition;
    jumpIfFalseStatementNode->label = label;
    return createNode(JumpIfFalse, (void *)jumpIfFalseStatementNode);
}

void freeJumpIfFalseStatementNode(JumpIfFalseStatementNode *node)
{
    freeNode(node->condition);
    node->condition = NULL;
    free(node);
}

Node *createJumpStatementNode(int label)
{
    JumpStatementNode *jumpStatementNode = (JumpStatementNode *)malloc(sizeof(JumpStatementNode));
    jumpStatementNode->label = label;
    return createNode(Jump, (void *)jumpStatementNode);
}

void freeJumpStatementNode(JumpStatementNode *node)
{
    free(node);
}

Node *createScopeNode(int inhert)
{
    ScopeNode *scopeNode = (ScopeNode *)malloc(sizeof(ScopeNode));
    scopeNode->inhert = inhert;
    scopeNode->head = NULL;
    scopeNode->tail = NULL;
    return createNode(Scope, (void *)scopeNode);
}

void appendNodeToScope(Node *scope, Node *node)
{
    ScopeNode *scopeNode = (ScopeNode *)scope->data;
    if (scopeNode->head == NULL)
    {
        scopeNode->head = scopeNode->tail = node;
    }
    else
    {
        scopeNode->tail->next = node;
        node->prev = scopeNode->tail;
        scopeNode->tail = scopeNode->tail->next;
    }
}

void freeScopeNode(ScopeNode *node)
{
    Node *p = node->tail;
    Node *prev = NULL;
    while (p != NULL)
    {
        prev = p->prev;
        freeNode(p);
        p = prev;
    }
    free(node);
}
