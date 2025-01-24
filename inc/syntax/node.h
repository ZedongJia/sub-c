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
    Compound,
    Program
} NodeType;
char *getNodeTypeValue(NodeType nodeType);
typedef struct Node
{
    NodeType nodeType;
    void *data; // allow NULL
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
typedef struct BinaryOperatorExpressionNode
{
    Node *left;
    TokenType op;
    Node *right;
} BinaryOperatorExpressionNode;
Node *createBinaryOperatorExpressionNode(Node *left, TokenType op, Node *right);
void freeBinaryOperatorExpressionNode(BinaryOperatorExpressionNode *node);
typedef struct UnaryOperatorExpressionNode
{
    TokenType op;
    Node *operand;
} UnaryOperatorExpressionNode;
Node *createUnaryOperatorExpressionNode(TokenType op, Node *operand);
void freeUnaryOperatorExpressionNode(UnaryOperatorExpressionNode *node);
typedef struct StatementNode
{
    Node *expression;
    TokenType delimiter;
} StatementNode;
Node *createStatementNode(Node *expression, TokenType delimiter);
void freeStatementNode(StatementNode *node);
typedef struct DeclarationNode
{
    Node *type;
    Node *expression;
    TokenType delimiter;
} DeclarationNode;
Node *createDeclarationNode(Node *type, Node *expression, TokenType delimiter);
void freeDeclarationNode(DeclarationNode *node);
typedef struct CompoundNode
{
    TokenType openDelimiter;
    Node **statements; // allow NULL
    int size;
    TokenType closeDelimiter;
} CompoundNode;
Node *createCompoundNode(TokenType openDelimiter, Node **statements, int size, TokenType closeDelimiter);
void freeCompoundNode(CompoundNode *node);
typedef struct ProgramNode
{
    Node **statements; // allow NULL
    int size;
    TokenType endDelimiter;
} ProgramNode;
Node *createProgramNode(Node **statements, int size, TokenType endDelimiter);
void freeProgramNode(ProgramNode *node);
#endif