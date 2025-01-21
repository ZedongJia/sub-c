#ifndef NODE
#define NODE
#include "token.h"
enum NodeType
{
    Err,
    Literal,
    BinaryOperator,
    UnaryOperator
};

char *getNodeTypeValue(enum NodeType nodeType);

struct Node
{
    enum NodeType nodeType;
    void *data;
};

struct LiteralNode
{
    char *value;
};

struct BinaryOperatorNode
{
    enum TokenType op;
    struct Node *left;
    struct Node *right;
};

struct UnaryOperatorNode
{
    enum TokenType op;
    struct Node *operand;
};

struct Node *createNode(enum NodeType nodeType, void *data);
struct Node *createLiteralNode(char *value, int valueLength);
struct Node *createBinaryOperatorNode(struct Node *left, enum TokenType op, struct Node *right);
struct Node *createUnaryOperatorNode(enum TokenType op, struct Node *operand);
void freeNode(struct Node *node);
void freeLiteralNode(struct LiteralNode *node);
void freeBinaryOperatorNode(struct BinaryOperatorNode *node);
#endif