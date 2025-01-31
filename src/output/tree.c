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
    __prettyTree(node, parr, 0, 1);
}

void __prettyNodeType(NodeType nodeType)
{
    printf("\033[33;1m%s\033[0m", getNodeTypeValue(nodeType));
}

void __prettyTokenType(TokenType type)
{
    printf("\033[35;1m%s\033[0m", getTokenTypeValue(type));
}

void __prettyType(BaseType baseType)
{
    printf("\033[36;1m%s\033[0m", getBaseTypeValue(baseType));
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
    case UNEXPECTED_NODE: {
        printf("\n");
        break;
    }
    case TYPE_NODE: {
        Type *type = (Type *)node;
        __prettyType(type->baseType);
        printf("\n");
        break;
    }
    case LITERAL_NODE: {
        Literal *literal = (Literal *)node;
        __prettyTokenType(literal->tokenType);
        printf("(%s)\n", literal->value);
        break;
    }
    case BINARY_OPERATE_NODE: {
        BinaryOperator *binaryOperator = (BinaryOperator *)node;
        __prettyTokenType(binaryOperator->tokenType);
        printf("\n");
        __prettyTree(binaryOperator->left, parr, indent + 1, 0);
        __prettyTree(binaryOperator->right, parr, indent + 1, 1);
        break;
    }
    case UNARY_OPERATE_NODE: {
        UnaryOperator *unaryOperator = (UnaryOperator *)node;
        __prettyTokenType(unaryOperator->tokenType);
        printf("\n");
        __prettyTree(unaryOperator->operand, parr, indent + 1, 1);
        break;
    }
    case DECLARATION_NODE: {
        Declaration *declaration = (Declaration *)node;
        printf("\n");
        __prettyTree(declaration->type, parr, indent + 1, 0);
        __prettyTree(declaration->identifier, parr, indent + 1, declaration->initializer == NULL);
        if (declaration->initializer != NULL)
            __prettyTree(declaration->initializer, parr, indent + 1, 1);
        break;
    }
    case LABEL_NODE: {
        Label *label = (Label *)node;
        printf("%d\n", label->number);
        break;
    }
    case JUMP_IF_FALSE_NODE: {
        JumpIfFalse *jumpIfFalse = (JumpIfFalse *)node;
        printf("%d\n", jumpIfFalse->number);
        __prettyTree(jumpIfFalse->condition, parr, indent + 1, 1);
        break;
    case JUMP_NODE: {
        Jump *jump = (Jump *)node;
        printf("%d\n", jump->number);
        break;
    }
    case SCOPE_NODE: {
        Scope *scope = (Scope *)node;
        printf("\n");
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