#include "syntax/node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getNodeTypeValue(NodeType nodeType)
{
    switch (nodeType)
    {
    case UNEXPECTED_NODE:
        return "?";
    case LITERAL_NODE:
        return "Literal";
    case BINARY_OPERATE_NODE:
        return "Binary Operator";
    case UNARY_OPERATE_NODE:
        return "Unary Operator";
    case DECLARATION_NODE:
        return "Declaration";
    case LABEL_NODE:
        return "Label";
    case JUMP_IF_FALSE_NODE:
        return "Jump If False";
    case JUMP_NODE:
        return "Jump";
    case SCOPE_NODE:
        return "Scope";
    default:
        return "?";
    }
}

Node *createNode(NodeType nodeType)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->nodeType = nodeType;
    return node;
}

void freeNode(void *node)
{
    // free data
    switch (((Node *)node)->nodeType)
    {
    case LITERAL_NODE:
        freeLiteral((Literal *)node);
        break;
    case BINARY_OPERATE_NODE:
        freeBinaryOperator((BinaryOperator *)node);
        break;
    case UNARY_OPERATE_NODE:
        freeUnaryOperator((UnaryOperator *)node);
        break;
    case DECLARATION_NODE:
        freeDeclaration((Declaration *)node);
        break;
    case LABEL_NODE:
        freeLabel((Label *)node);
        break;
    case JUMP_IF_FALSE_NODE:
        freeJumpIfFalse((JumpIfFalse *)node);
        break;
    case JUMP_NODE:
        freeJump((Jump *)node);
        break;
    case SCOPE_NODE:
        freeScope((Scope *)node);
        break;
    default:
        free(node);
        break;
    }
}

Node *createLiteral(BaseType *baseType, TokenType tokenType, char *value)
{
    Literal *literal = (Literal *)malloc(sizeof(Literal));
    literal->nodeType = LITERAL_NODE;
    literal->baseType = baseType;
    literal->tokenType = tokenType;
    literal->value = (char *)malloc((strlen(value) + 1) * sizeof(char));
    strcpy(literal->value, value);
    return (Node *)literal;
}

void freeLiteral(Literal *node)
{
    free(node->value);
    node->value = NULL;
    freeBaseType(node->baseType);
    node->baseType = NULL;
    free(node);
}

Node *createUnaryOperator(BaseType *baseType, TokenType tokenType, Node *operand)
{
    UnaryOperator *unaryOperator = (UnaryOperator *)malloc(sizeof(UnaryOperator));
    unaryOperator->nodeType = UNARY_OPERATE_NODE;
    unaryOperator->baseType = baseType;
    unaryOperator->tokenType = tokenType;
    unaryOperator->operand = operand;
    return (Node *)unaryOperator;
}

void freeUnaryOperator(UnaryOperator *node)
{
    freeBaseType(node->baseType);
    node->baseType = NULL;
    freeNode(node->operand);
    node->operand = NULL;
    free(node);
}

Node *createBinaryOperator(BaseType *baseType, Node *left, TokenType tokenType, Node *right)
{
    BinaryOperator *binaryOperator = (BinaryOperator *)malloc(sizeof(BinaryOperator));
    binaryOperator->nodeType = BINARY_OPERATE_NODE;
    binaryOperator->baseType = baseType;
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
    freeBaseType(node->baseType);
    node->baseType = NULL;
    free(node);
}

Node *createDeclaration(BaseType *baseType, const char *name)
{
    Declaration *declaration = (Declaration *)malloc(sizeof(Declaration));
    declaration->nodeType = DECLARATION_NODE;
    declaration->baseType = baseType;
    declaration->name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    strcpy(declaration->name, name);
    return (Node *)declaration;
}

void freeDeclaration(Declaration *node)
{
    freeBaseType(node->baseType);
    node->baseType = NULL;
    freeNode(node->name);
    node->name = NULL;
    free(node);
}

Node *createLabel(int number)
{
    Label *labelNumber = (Label *)malloc(sizeof(Label));
    labelNumber->nodeType = LABEL_NODE;
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
    jumpIfFalse->nodeType = JUMP_IF_FALSE_NODE;
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
    jump->nodeType = JUMP_NODE;
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
    scope->nodeType = SCOPE_NODE;
    scope->table.num_var = 0;
    scope->parentScope = parentScope;
    scope->list = createList();
    return (Node *)scope;
}

void freeScope(Scope *node)
{
    freeList(node->list, freeNode);
    free(node);
}
