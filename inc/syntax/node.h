#ifndef NODE
#define NODE
#include "syntax/token.h"
typedef enum NodeType
{
    // expression

    Err,
    TypeExpression,
    LiteralExpression,
    BinaryOperatorExpression,
    UnaryOperatorExpression,

    // statement

    Statement,
    Declaration,
    Label,
    JumpIfFalse,
    Jump,
    Scope
} NodeType;
char *getNodeTypeValue(NodeType nodeType);

// expression

typedef struct Node
{
    NodeType nodeType;
    void *data; // allow NULL

    // preserve for scope

    struct Node *next;
    struct Node *prev;
} Node;
Node *createNode(NodeType nodeType, void *data);
void freeNode(Node *node);

typedef struct TypeExpressionNode
{
    TokenType type;
} TypeExpressionNode;
Node *createTypeExpressionNode(TokenType type);
void freeTypeExpressionNode(TypeExpressionNode *node);

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

// statement

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

typedef struct LabelStatementNode
{
    int label;
} LabelStatementNode;
Node *createLabelStatementNode(int label);
void freeLabelStatementNode(LabelStatementNode *node);

typedef struct JumpIfFalseStatementNode
{
    Node *condition;
    int label;
} JumpIfFalseStatementNode;
Node *createJumpIfFalseStatementNode(Node *condition, int label);
void freeJumpIfFalseStatementNode(JumpIfFalseStatementNode *node);

typedef struct JumpStatementNode
{
    int label;
} JumpStatementNode;
Node *createJumpStatementNode(int label);
void freeJumpStatementNode(JumpStatementNode *node);

typedef struct ScopeNode
{
    int inhert;
    Node *head;
    Node *tail;
} ScopeNode;
Node *createScopeNode(int inhert);
void appendNodeToScope(Node *scope, Node *node);
void freeScopeNode(ScopeNode *node);
#endif