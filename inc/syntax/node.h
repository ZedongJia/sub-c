#ifndef NODE
#define NODE
#include "collection/list.h"
#include "syntax/token.h"

typedef enum NodeKind
{
    // expression
    UNEXPECTED,
    TYPE,
    LITERAL,
    BINARY_OPERATE,
    UNARY_OPERATE,
    // statement
    DECLARATION,
    LABEL,
    JUMP_IF_FALSE,
    JUMP,
    SCOPE
} NodeKind;
char *getNodeKindValue(NodeKind kind);

typedef struct Node
{
    NodeKind kind;
} Node;
Node *createNode(NodeKind kind);
void freeNode(void *node);

typedef struct Type
{
    NodeKind kind;
    TokenType baseType;
} Type;
Node *createType(TokenType baseType);
void freeType(Type *node);

typedef struct Literal
{
    NodeKind kind;
    TokenType type;
    char *value;
    int length;
} Literal;
Node *createLiteral(TokenType type, char *value, int length);
void freeLiteral(Literal *node);

typedef struct UnaryOperator
{
    NodeKind kind;
    TokenType type;
    Node *operand;
} UnaryOperator;
Node *createUnaryOperator(TokenType type, Node *operand);
void freeUnaryOperator(UnaryOperator *node);

typedef struct BinaryOperator
{
    NodeKind kind;
    Node *left;
    TokenType type;
    Node *right;
} BinaryOperator;
Node *createBinaryOperator(Node *left, TokenType type, Node *right);
void freeBinaryOperator(BinaryOperator *node);

typedef struct Declaration
{
    NodeKind kind;
    Node *type;
    Node *expression;
} Declaration;
Node *createDeclaration(Node *type, Node *expression);
void freeDeclaration(Declaration *node);

typedef struct Label
{
    NodeKind kind;
    int number;
} Label;
Node *createLabel(int number);
void freeLabel(Label *node);

typedef struct JumpIfFalse
{
    NodeKind kind;
    Node *condition;
    int number;
} JumpIfFalse;
Node *createJumpIfFalse(Node *condition, int number);
void freeJumpIfFalse(JumpIfFalse *node);

typedef struct Jump
{
    NodeKind kind;
    int number;
} Jump;
Node *createJump(int number);
void freeJump(Jump *node);

typedef struct Scope
{
    NodeKind kind;
    int inhert;
    List *list;
} Scope;
Node *createScope(int inhert);
void freeScope(Scope *node);
#endif