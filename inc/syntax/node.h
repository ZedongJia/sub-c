#ifndef NODE
#define NODE
#include "collection/list.h"
#include "syntax/token.h"
#include "syntax/types.h"

typedef enum NodeKind
{
    // expression
    UNEXPECTED_KIND,
    TYPE_KIND,
    LITERAL_KIND,
    BINARY_OPERATE_KIND,
    UNARY_OPERATE_KIND,
    // statement
    DECLARATION_KIND,
    LABEL_KIND,
    JUMP_IF_FALSE_KIND,
    JUMP_KIND,
    SCOPE_KIND
} NodeKind;
char *getNodeKindValue(NodeKind kind);

typedef struct Node
{
    // inhert
    NodeKind kind;
} Node;
Node *createNode(NodeKind kind);
void freeNode(void *node);

typedef struct Type
{
    // inhert
    NodeKind kind;
    // extend
    BaseType baseType;
} Type;
Node *createType(BaseType baseType);
void freeType(Type *node);

typedef struct Literal
{
    // inhert
    NodeKind kind;
    // extend
    TokenType type;
    char *value;
    int length;
} Literal;
Node *createLiteral(TokenType type, char *value, int length);
void freeLiteral(Literal *node);

typedef struct UnaryOperator
{
    // inhert
    NodeKind kind;
    // extend
    TokenType type;
    Node *operand;
} UnaryOperator;
Node *createUnaryOperator(TokenType type, Node *operand);
void freeUnaryOperator(UnaryOperator *node);

typedef struct BinaryOperator
{
    // inhert
    NodeKind kind;
    // extend
    Node *left;
    TokenType type;
    Node *right;
} BinaryOperator;
Node *createBinaryOperator(Node *left, TokenType type, Node *right);
void freeBinaryOperator(BinaryOperator *node);

typedef struct Declaration
{
    // inhert
    NodeKind kind;
    // extend
    Node *type;
    Node *identifier;
    Node *initializer; // allow NULL
} Declaration;
Node *createDeclaration(Node *type, Node *identifier, Node *initializer);
void freeDeclaration(Declaration *node);

typedef struct Label
{
    // inhert
    NodeKind kind;
    // extend
    int number;
} Label;
Node *createLabel(int number);
void freeLabel(Label *node);

typedef struct JumpIfFalse
{
    // inhert
    NodeKind kind;
    // extend
    Node *condition;
    int number;
} JumpIfFalse;
Node *createJumpIfFalse(Node *condition, int number);
void freeJumpIfFalse(JumpIfFalse *node);

typedef struct Jump
{
    // inhert
    NodeKind kind;
    // extend
    int number;
} Jump;
Node *createJump(int number);
void freeJump(Jump *node);

typedef struct Scope
{
    // inhert
    NodeKind kind;
    // extend
    List *list;
    struct Scope *parentScope; // parent reference
} Scope;
Node *createScope(Scope *parentScope);
void freeScope(Scope *node);
#endif