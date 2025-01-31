#include "syntax/node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getNodeKindValue(NodeKind kind)
{
    switch (kind)
    {
    case UNEXPECTED_KIND:
        return "?";
    case TYPE_KIND:
        return "Type";
    case LITERAL_KIND:
        return "Literal";
    case BINARY_OPERATE_KIND:
        return "Binary Operator";
    case UNARY_OPERATE_KIND:
        return "Unary Operator";
    case DECLARATION_KIND:
        return "Declaration";
    case LABEL_KIND:
        return "Label";
    case JUMP_IF_FALSE_KIND:
        return "Jump If False";
    case JUMP_KIND:
        return "Jump";
    case SCOPE_KIND:
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
    case TYPE_KIND:
        freeType((Type *)node);
        break;
    case LITERAL_KIND:
        freeLiteral((Literal *)node);
        break;
    case BINARY_OPERATE_KIND:
        freeBinaryOperator((BinaryOperator *)node);
        break;
    case UNARY_OPERATE_KIND:
        freeUnaryOperator((UnaryOperator *)node);
        break;
    case DECLARATION_KIND:
        freeDeclaration((Declaration *)node);
        break;
    case LABEL_KIND:
        freeLabel((Label *)node);
        break;
    case JUMP_IF_FALSE_KIND:
        freeJumpIfFalse((JumpIfFalse *)node);
        break;
    case JUMP_KIND:
        freeJump((Jump *)node);
        break;
    case SCOPE_KIND:
        freeScope((Scope *)node);
        break;
    default:
        free(node);
        break;
    }
}

Node *createType(BaseType baseType)
{
    Type *type = (Type *)malloc(sizeof(Type));
    type->kind = TYPE_KIND;
    type->baseType = baseType;
    return (Node *)type;
}

void freeType(Type *node)
{
    free(node);
}

Node *createLiteral(TokenType tokenType, char *value)
{
    Literal *literal = (Literal *)malloc(sizeof(Literal));
    literal->kind = LITERAL_KIND;
    literal->tokenType = tokenType;
    literal->value = (char *)malloc((strlen(value) + 1) * sizeof(char));
    strcpy(literal->value, value);
    return (Node *)literal;
}

void freeLiteral(Literal *node)
{
    if (node->value != NULL)
        free(node->value);
    node->value = NULL;
    free(node);
}

Node *createUnaryOperator(TokenType tokenType, Node *operand)
{
    UnaryOperator *unaryOperator = (UnaryOperator *)malloc(sizeof(UnaryOperator));
    unaryOperator->kind = UNARY_OPERATE_KIND;
    unaryOperator->tokenType = tokenType;
    unaryOperator->operand = operand;
    return (Node *)unaryOperator;
}

void freeUnaryOperator(UnaryOperator *node)
{
    freeNode(node->operand);
    node->operand = NULL;
    free(node);
}

Node *createBinaryOperator(Node *left, TokenType tokenType, Node *right)
{
    BinaryOperator *binaryOperator = (BinaryOperator *)malloc(sizeof(BinaryOperator));
    binaryOperator->kind = BINARY_OPERATE_KIND;
    binaryOperator->left = left;
    binaryOperator->tokenType = tokenType;
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

Node *createDeclaration(Node *type, Node *identifier, Node *initializer)
{
    Declaration *declaration = (Declaration *)malloc(sizeof(Declaration));
    declaration->kind = DECLARATION_KIND;
    declaration->type = type;
    declaration->identifier = identifier;
    declaration->initializer = initializer;
    return (Node *)declaration;
}

void freeDeclaration(Declaration *node)
{
    freeNode(node->type);
    node->type = NULL;
    freeNode(node->identifier);
    node->identifier = NULL;
    if (node->initializer != NULL)
        freeNode(node->initializer);
    node->initializer = NULL;
    free(node);
}

Node *createLabel(int number)
{
    Label *labelNumber = (Label *)malloc(sizeof(Label));
    labelNumber->kind = LABEL_KIND;
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
    jumpIfFalse->kind = JUMP_IF_FALSE_KIND;
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
    jump->kind = JUMP_KIND;
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
    scope->kind = SCOPE_KIND;
    scope->parentScope = parentScope;
    scope->list = createList();
    return (Node *)scope;
}

void freeScope(Scope *node)
{
    freeList(node->list, freeNode);
    free(node);
}
