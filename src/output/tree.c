#include "output/tree.h"
#include "output/color.h"
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

void __prettyNodeType(NodeKind nodeType)
{
    setColor(GREEN);
    printf("%s", getNodeKindValue(nodeType));
    clearColor();
}

void __prettyTokenType(TokenType type)
{
    setColor(BLUE);
    printf("%s", getTokenTypeValue(type));
    clearColor();
}

void __prettyType(BaseType baseType)
{
    setColor(PURPLE);
    printf("%s", getBaseTypeValue(baseType));
    clearColor();
}

void __prettyTree(Node *node, int *parr, int indent, int isLast)
{
    for (int i = 0; i < indent; i++)
        printf(parr[i] ? "    " : "│   ");
    parr[indent] = isLast ? 1 : 0;
    printf(isLast ? "└── " : "├── ");
    __prettyNodeType(node->kind);
    printf(" ");
    switch (node->kind)
    {
    case UNEXPECTED_KIND: {
        printf("\n");
        break;
    }
    case TYPE_KIND: {
        Type *type = (Type *)node;
        __prettyType(type->baseType);
        printf("\n");
        break;
    }
    case LITERAL_KIND: {
        Literal *literal = (Literal *)node;
        __prettyTokenType(literal->type);
        printf("(%s)\n", literal->value);
        break;
    }
    case BINARY_OPERATE_KIND: {
        BinaryOperator *binaryOperator = (BinaryOperator *)node;
        __prettyTokenType(binaryOperator->type);
        printf("\n");
        __prettyTree(binaryOperator->left, parr, indent + 1, 0);
        __prettyTree(binaryOperator->right, parr, indent + 1, 1);
        break;
    }
    case UNARY_OPERATE_KIND: {
        UnaryOperator *unaryOperator = (UnaryOperator *)node;
        __prettyTokenType(unaryOperator->type);
        printf("\n");
        __prettyTree(unaryOperator->operand, parr, indent + 1, 1);
        break;
    }
    case DECLARATION_KIND: {
        Declaration *declaration = (Declaration *)node;
        printf("\n");
        __prettyTree(declaration->type, parr, indent + 1, 0);
        __prettyTree(declaration->identifier, parr, indent + 1, declaration->initializer == NULL);
        if (declaration->initializer != NULL)
            __prettyTree(declaration->initializer, parr, indent + 1, 1);
        break;
    }
    case LABEL_KIND: {
        Label *label = (Label *)node;
        printf("%d\n", label->number);
        break;
    }
    case JUMP_IF_FALSE_KIND: {
        JumpIfFalse *jumpIfFalse = (JumpIfFalse *)node;
        printf("%d\n", jumpIfFalse->number);
        __prettyTree(jumpIfFalse->condition, parr, indent + 1, 1);
        break;
    case JUMP_KIND: {
        Jump *jump = (Jump *)node;
        printf("%d\n", jump->number);
        break;
    }
    case SCOPE_KIND: {
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