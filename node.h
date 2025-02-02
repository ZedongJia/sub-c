#include "list.h"
#include "symbol.h"
#include "token.h"
#include "types.h"
#ifndef NODE
#define NODE

typedef enum NodeType
{
    // expression
    UNEXPECTED_NODE,
    LITERAL_NODE,
    BINARY_OPERATE_NODE,
    UNARY_OPERATE_NODE,
    // statement
    DECLARATION_NODE,
    LABEL_NODE,
    JUMP_IF_FALSE_NODE,
    JUMP_NODE,
    SCOPE_NODE
} NodeType;
char *getNodeTypeValue(NodeType nodeType);

typedef struct Node
{
    // inhert
    NodeType nodeType;
} Node;
Node *createNode(NodeType nodeType);
void freeNode(void *node);

typedef struct ExpressionView
{
    // inhert
    NodeType nodeType;
    BaseType *baseType; // ref
} ExpressionView;

typedef struct Literal
{
    // inhert
    NodeType nodeType;
    // inhert from expression
    BaseType *baseType; // ref
    // extend
    TokenType tokenType;
    char *value;
} Literal;
Node *createLiteral(BaseType *baseType, TokenType tokenType, char *value);
void freeLiteral(Literal *node);

typedef struct UnaryOperator
{
    // inhert
    NodeType nodeType;
    // inhert from expression
    BaseType *baseType; // ref
    // extend
    TokenType tokenType;
    Node *operand;
} UnaryOperator;
Node *createUnaryOperator(BaseType *baseType, TokenType tokenType, Node *operand);
void freeUnaryOperator(UnaryOperator *node);

typedef struct BinaryOperator
{
    // inhert
    NodeType nodeType;
    // inhert from expression
    BaseType *baseType; // ref
    // extend
    Node *left;
    TokenType tokenType;
    Node *right;
} BinaryOperator;
Node *createBinaryOperator(BaseType *baseType, Node *left, TokenType tokenType, Node *right);
void freeBinaryOperator(BinaryOperator *node);

typedef struct Declaration
{
    // inhert
    NodeType nodeType;
    // extend
    BaseType *baseType;
    char *name;
} Declaration;
Node *createDeclaration(BaseType *baseType, const char *name);
void freeDeclaration(Declaration *node);

typedef struct Label
{
    // inhert
    NodeType nodeType;
    // extend
    int number;
} Label;
Node *createLabel(int number);
void freeLabel(Label *node);

typedef struct JumpIfFalse
{
    // inhert
    NodeType nodeType;
    // extend
    Node *condition;
    int number;
} JumpIfFalse;
Node *createJumpIfFalse(Node *condition, int number);
void freeJumpIfFalse(JumpIfFalse *node);

typedef struct Jump
{
    // inhert
    NodeType nodeType;
    // extend
    int number;
} Jump;
Node *createJump(int number);
void freeJump(Jump *node);

typedef struct Scope
{
    // inhert
    NodeType nodeType;
    // extend
    SymbolTable table;
    List *list;
    struct Scope *parentScope; // parent reference
} Scope;
Node *createScope(Scope *parentScope);
void freeScope(Scope *node);
#endif