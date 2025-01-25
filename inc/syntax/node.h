#ifndef NODE
#define NODE
#include "syntax/token.h"
typedef enum NodeType
{
    Err,
    KeywordExpression,
    LiteralExpression,
    BinaryOperatorExpression,
    UnaryOperatorExpression,
    Statement,
    Declaration,
} NodeType;
char *getNodeTypeValue(NodeType nodeType);
typedef struct Node
{
    NodeType nodeType;
    void *data; // allow NULL
    struct Node *next;
} Node;
Node *createNode(NodeType nodeType, void *data);
void freeNode(Node *node);
typedef struct KeywordExpressionNode
{
    TokenType keywordType;
} KeywordExpressionNode;
Node *createKeywordExpressionNode(TokenType keywordType);
void freeKeywordExpressionNode(KeywordExpressionNode *node);
typedef struct LiteralExpressionNode
{
    TokenType literalType;
    char *value;
    int length;
} LiteralExpressionNode;
Node *createLiteralExpressionNode(TokenType literalType, char *value, int length);
void freeLiteralExpressionNode(LiteralExpressionNode *node);
typedef struct UnaryOperatorExpressionNode
{
    TokenType op;
    Node *operand;
} UnaryOperatorExpressionNode;
Node *createUnaryOperatorExpressionNode(TokenType op, Node *operand);
void freeUnaryOperatorExpressionNode(UnaryOperatorExpressionNode *node);
typedef struct BinaryOperatorExpressionNode
{
    Node *left;
    TokenType op;
    Node *right;
} BinaryOperatorExpressionNode;
Node *createBinaryOperatorExpressionNode(Node *left, TokenType op, Node *right);
void freeBinaryOperatorExpressionNode(BinaryOperatorExpressionNode *node);
typedef struct StatementNode
{
    Node *expression;
} StatementNode;
Node *createStatementNode(Node *expression);
void freeStatementNode(StatementNode *node);
typedef struct DeclarationNode
{
    Node *type;
    Node *expression;
} DeclarationNode;
Node *createDeclarationNode(Node *type, Node *expression);
void freeDeclarationNode(DeclarationNode *node);
#endif