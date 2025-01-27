#include "debug.h"
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
    printf("\033[32;1m%s\033[0m", getNodeKindValue(nodeType));
}

void __prettyTokenType(TokenType tokenType)
{
    printf("\033[34;1m%s\033[0m", getTokenTypeValue(tokenType));
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
    case UNEXPECTED: {
        printf("\n");
        break;
    }
    case TYPE: {
        Type *keywordNode = (Type *)node;
        __prettyTokenType(keywordNode->baseType);
        printf("\n");
        break;
    }
    case LITERAL: {
        Literal *literalNode = (Literal *)node;
        __prettyTokenType(literalNode->type);
        printf("(%s)\n", literalNode->value);
        break;
    }
    case BINARY_OPERATE: {
        BinaryOperator *binaryOperatorNode = (BinaryOperator *)node;
        __prettyTokenType(binaryOperatorNode->type);
        printf("\n");
        __prettyTree(binaryOperatorNode->left, parr, indent + 1, 0);
        __prettyTree(binaryOperatorNode->right, parr, indent + 1, 1);
        break;
    }
    case UNARY_OPERATE: {
        UnaryOperator *unaryOperatorNode = (UnaryOperator *)node;
        __prettyTokenType(unaryOperatorNode->type);
        printf("\n");
        __prettyTree(unaryOperatorNode->operand, parr, indent + 1, 1);
        break;
    }
    case DECLARATION: {
        Declaration *declarationNode = (Declaration *)node;
        printf("\n");
        __prettyTree(declarationNode->type, parr, indent + 1, 0);
        __prettyTree(declarationNode->expression, parr, indent + 1, 1);
        break;
    }
    case LABEL: {
        Label *labelStatementNode = (Label *)node;
        printf("%d\n", labelStatementNode->number);
        break;
    }
    case JUMP_IF_FALSE: {
        JumpIfFalse *jumpIfFalseStatementNode = (JumpIfFalse *)node;
        printf("%d\n", jumpIfFalseStatementNode->number);
        __prettyTree(jumpIfFalseStatementNode->condition, parr, indent + 1, 1);
        break;
    case JUMP: {
        Jump *jumpStatementNode = (Jump *)node;
        printf("%d\n", jumpStatementNode->number);
        break;
    }
    case SCOPE: {
        Scope *scopeNode = (Scope *)node;
        printf("\n");
        ListNode *p = scopeNode->list->head;
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