#include "output/tree.h"
#include <stdio.h>
#include <string.h>

void prettyTree(Node *node)
{
    if (node == NULL)
        return;
    int parr[256];
    for (int i = 0; i < 256; i++)
        parr[i] = 0;
    
    printf("[Tree]");
    __prettyTree(node, parr, 0, 1);
    printf("\n");
}

void __prettyPrefix(int *parr, int indent, int isLast, const char *end)
{
    printf("\n");
    for (int i = 0; i < indent; i++)
        printf(parr[i] ? "    " : "│   ");
    printf(isLast ? "└─%s " : "├─%s ", end);
}

void __prettyNodeType(NodeType nodeType)
{
    printf("\033[33;1m%s\033[0m", getNodeTypeValue(nodeType));
}

void __prettyTokenType(TokenType type)
{
    printf("\033[35;1m%s\033[0m", getTokenTypeValue(type));
}

void __prettyPropertyName(const char *name)
{
    printf("\033[36;1m%s\033[0m", name);
}

void __prettyBaseType(BaseType *baseType)
{
    printf("%s", getValueTypeValue(baseType->valueType));
    switch (baseType->valueType)
    {
    case POINTER_VALUE: {
        PointerType *pointerType = (PointerType *)baseType;
        printf("_%d(", pointerType->dim);
        __prettyBaseType(pointerType->baseType);
        printf(")");
        break;
    }
    case ARRAY_VALUE: {
        ArrayType *arrayType = (ArrayType *)baseType;
        printf("(%d*", arrayType->size);
        __prettyBaseType(arrayType->baseType);
        printf(")");
        break;
    }
    default:
        break;
    }
}

void __prettyTree(Node *node, int *parr, int indent, int isLast)
{
    __prettyPrefix(parr, indent, isLast, "─");
    parr[indent] = isLast ? 1 : 0;
    __prettyNodeType(node->nodeType);
    printf(" ");
    switch (node->nodeType)
    {
    case LITERAL_NODE: {
        Literal *literal = (Literal *)node;
        __prettyTokenType(literal->tokenType);
        printf("(%s)", literal->value);
        break;
    }
    case BINARY_OPERATE_NODE: {
        BinaryOperator *binaryOperator = (BinaryOperator *)node;
        __prettyTokenType(binaryOperator->tokenType);
        __prettyTree(binaryOperator->left, parr, indent + 1, 0);
        __prettyTree(binaryOperator->right, parr, indent + 1, 1);
        break;
    }
    case UNARY_OPERATE_NODE: {
        UnaryOperator *unaryOperator = (UnaryOperator *)node;
        __prettyTokenType(unaryOperator->tokenType);
        __prettyTree(unaryOperator->operand, parr, indent + 1, 1);
        break;
    }
    case DECLARATION_NODE: {
        Declaration *declaration = (Declaration *)node;
        // type
        __prettyPrefix(parr, indent + 1, 0, "*");
        __prettyPropertyName("Type: ");
        __prettyBaseType(declaration->baseType);
        printf(" %dbytes ", declaration->baseType->offset);
        // identifier
        __prettyPrefix(parr, indent + 1, 1, "*");
        __prettyPropertyName("Identifier: ");
        printf("%s", declaration->name);
        break;
    }
    case LABEL_NODE: {
        Label *label = (Label *)node;
        printf("%d", label->number);
        break;
    }
    case JUMP_IF_FALSE_NODE: {
        JumpIfFalse *jumpIfFalse = (JumpIfFalse *)node;
        printf("%d", jumpIfFalse->number);
        __prettyTree(jumpIfFalse->condition, parr, indent + 1, 1);
        break;
    case JUMP_NODE: {
        Jump *jump = (Jump *)node;
        printf("%d", jump->number);
        break;
    }
    case SCOPE_NODE: {
        Scope *scope = (Scope *)node;
        ListNode *p = scope->list->head;
        while (p != NULL)
        {
            __prettyTree((Node *)p->data, parr, indent + 1, p->next == NULL);
            p = p->next;
        }
    }
    default:
        break;
    }
    }
}