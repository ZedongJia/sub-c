#ifndef NODE
#define NODE
#include "token.h"
enum NodeType
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
};
char *getNodeTypeValue(enum NodeType nodeType);
struct Node
{
    enum NodeType nodeType;
    void *data; // allow NULL
};
struct Node *createNode(enum NodeType nodeType, void *data);
void freeNode(struct Node *node);
struct KeywordExpressionNode
{
    enum TokenType keywordType;
};
struct Node *createKeywordExpressionNode(enum TokenType keywordType);
void freeKeywordExpressionNode(struct KeywordExpressionNode *node);
struct LiteralExpressionNode
{
    enum TokenType literalType;
    char *value;
    int length;
};
struct Node *createLiteralExpressionNode(enum TokenType literalType, char *value, int length);
void freeLiteralExpressionNode(struct LiteralExpressionNode *node);
struct BinaryOperatorExpressionNode
{
    enum TokenType op;
    struct Node *left;
    struct Node *right;
};
struct Node *createBinaryOperatorExpressionNode(struct Node *left, enum TokenType op, struct Node *right);
void freeBinaryOperatorExpressionNode(struct BinaryOperatorExpressionNode *node);
struct UnaryOperatorExpressionNode
{
    enum TokenType op;
    struct Node *operand;
};
struct Node *createUnaryOperatorExpressionNode(enum TokenType op, struct Node *operand);
void freeUnaryOperatorExpressionNode(struct UnaryOperatorExpressionNode *node);
struct StatementNode
{
    struct Node *expression;
    enum TokenType delimiter;
};
struct Node *createStatementNode(struct Node *expression, enum TokenType delimiter);
void freeStatementNode(struct StatementNode *node);
struct DeclarationNode
{
    struct Node *type;
    struct Node *expression;
    enum TokenType delimiter;
};
struct Node *createDeclarationNode(struct Node *type, struct Node *expression, enum TokenType delimiter);
void freeDeclarationNode(struct DeclarationNode *node);
struct CompoundNode
{
    struct Node **statements; // allow NULL
    int size;
};
struct Node *createCompoundNode(struct Node **statements, int size);
void freeCompoundNode(struct CompoundNode *node);
struct ProgramNode
{
    struct Node **statements; // allow NULL
    int size;
};
struct Node *createProgramNode(struct Node **statements, int size);
void freeProgramNode(struct ProgramNode *node);
#endif