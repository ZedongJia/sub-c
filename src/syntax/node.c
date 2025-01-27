#include "syntax/node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getNodeKindValue(NodeKind kind)
{
    switch (kind)
    {
    case UNEXPECTED:
        return "?";
    case TYPE:
        return "Type";
    case LITERAL:
        return "Literal";
    case BINARY_OPERATE:
        return "Binary Operator";
    case UNARY_OPERATE:
        return "Unary Operator";
    case DECLARATION:
        return "Declaration";
    case LABEL:
        return "Label";
    case JUMP_IF_FALSE:
        return "Jump If False";
    case JUMP:
        return "Jump";
    case SCOPE:
        return "Scope";
    default:
        return "?";
    }
}

Node *createNode(NodeKind kind)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->kind = kind;
    return node;
}

void freeNode(void *node)
{
    // free data
    switch (((Node *)node)->kind)
    {
    case TYPE:
        freeType((Type *)node);
        break;
    case LITERAL:
        freeLiteral((Literal *)node);
        break;
    case BINARY_OPERATE:
        freeBinaryOperator((BinaryOperator *)node);
        break;
    case UNARY_OPERATE:
        freeUnaryOperator((UnaryOperator *)node);
        break;
    case DECLARATION:
        freeDeclaration((Declaration *)node);
        break;
    case LABEL:
        freeLabel((Label *)node);
        break;
    case JUMP_IF_FALSE:
        freeJumpIfFalse((JumpIfFalse *)node);
        break;
    case JUMP:
        freeJump((Jump *)node);
        break;
    case SCOPE:
        freeScope((Scope *)node);
        break;
    default:
        free(node);
        break;
    }
}

Node *createType(TokenType baseType)
{
    Type *type = (Type *)malloc(sizeof(Type));
    type->kind = TYPE;
    type->baseType = baseType;
    return (Node *)type;
}

void freeType(Type *node)
{
    free(node);
}

Node *createLiteral(TokenType type, char *value, int length)
{
    Literal *literal = (Literal *)malloc(sizeof(Literal));
    literal->kind = LITERAL;
    literal->type = type;
    literal->value = (char *)malloc(length * sizeof(char));
    strcpy(literal->value, value);
    literal->length = length;
    return (Node *)literal;
}

void freeLiteral(Literal *node)
{
    if (node->value != NULL)
        free(node->value);
    node->value = NULL;
    free(node);
}

Node *createUnaryOperator(TokenType type, Node *operand)
{
    UnaryOperator *unaryOperator = (UnaryOperator *)malloc(sizeof(UnaryOperator));
    unaryOperator->kind = UNARY_OPERATE;
    unaryOperator->type = type;
    unaryOperator->operand = operand;
    return (Node *)unaryOperator;
}

void freeUnaryOperator(UnaryOperator *node)
{
    freeNode(node->operand);
    node->operand = NULL;
    free(node);
}

Node *createBinaryOperator(Node *left, TokenType type, Node *right)
{
    BinaryOperator *binaryOperator = (BinaryOperator *)malloc(sizeof(BinaryOperator));
    binaryOperator->kind = BINARY_OPERATE;
    binaryOperator->left = left;
    binaryOperator->type = type;
    binaryOperator->right = right;
    return (Node *)binaryOperator;
}

void freeBinaryOperator(BinaryOperator *node)
{
    freeNode(node->left);
    node->left = NULL;
    freeNode(node->right);
    node->right = NULL;
    free(node);
}

Node *createDeclaration(Node *type, Node *expression)
{
    Declaration *declaration = (Declaration *)malloc(sizeof(Declaration));
    declaration->kind = DECLARATION;
    declaration->type = type;
    declaration->expression = expression;
    return (Node *)declaration;
}

void freeDeclaration(Declaration *node)
{
    freeNode(node->type);
    node->type = NULL;
    freeNode(node->expression);
    node->expression = NULL;
    free(node);
}

Node *createLabel(int number)
{
    Label *labelNumber = (Label *)malloc(sizeof(Label));
    labelNumber->kind = LABEL;
    labelNumber->number = number;
    return (Node *)labelNumber;
}

void freeLabel(Label *node)
{
    free(node);
}

Node *createJumpIfFalse(Node *condition, int number)
{
    JumpIfFalse *jumpIfFalse = (JumpIfFalse *)malloc(sizeof(JumpIfFalse));
    jumpIfFalse->kind = JUMP_IF_FALSE;
    jumpIfFalse->condition = condition;
    jumpIfFalse->number = number;
    return (Node *)jumpIfFalse;
}

void freeJumpIfFalse(JumpIfFalse *node)
{
    freeNode(node->condition);
    node->condition = NULL;
    free(node);
}

Node *createJump(int number)
{
    Jump *jump = (Jump *)malloc(sizeof(Jump));
    jump->kind = JUMP;
    jump->number = number;
    return (Node *)jump;
}

void freeJump(Jump *node)
{
    free(node);
}

Node *createScope(Scope *parentScope)
{
    Scope *scope = (Scope *)malloc(sizeof(Scope));
    scope->kind = SCOPE;
    scope->parentScope = parentScope;
    scope->list = createList();
    return (Node *)scope;
}

void freeScope(Scope *node)
{
    freeList(node->list, freeNode);
    free(node);
}
