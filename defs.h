#ifndef DEFS
#define DEFS
#include "enums.h"
#include "list.h"
#include <stdio.h>

/**
 *     Define Lexer
 */
struct Span
{
    int row;
    int col;
};

struct Lexer
{
    FILE *__in; // lex file
    char __pc;  // putback char
    char __cc;  // current char
    Token token;
    char text[255]; // buf
    int len;
    struct Span span; // value span

    void (*next)(struct Lexer *lexer);
    int (*match)(struct Lexer *lexer, Token what);
    void (*del)(struct Lexer *lexer);
};

struct Lexer *new_lexer(FILE *in);

/**
 *     Define struct CType
 */
struct CType
{
    Type type;
    int offset[16]; // max type recursive = 16
    int ptr;
    int mod;
    struct CType *(*clone)(struct CType *);
    void (*del)(struct CType *);
};

struct CType *new_CType(Type type, int mod);

/// @brief pointer to this ctype, mod inplace
void point(struct CType *ctype);

/// @brief depointer to this ctype, mod inplace
void depoint(struct CType *ctype);

/// @brief array for this ctype, mod inplace
void array(struct CType *ctype, int size);

/// @brief return widen one
struct CType *type_cast(struct CType *left, struct CType *right);

/// @brief check unary valid, if invalid return `NULL`
struct CType *unary_compatible(Kind kind, struct CType *left);

/// @brief check binary valid, if invalid return `NULL`
struct CType *binary_compatible(Kind kind, struct CType *left, struct CType *right);

/**
 *     Define Symbol Table
 */
struct VariableSymbol
{
    struct CType *ctype;
    const char *name;
};

struct SymbolTable
{
    struct VariableSymbol variables[256];
    int num_var;
};

int try_look_up(struct SymbolTable *table, const char *name);
int try_declare(struct SymbolTable *table, struct CType *ctype, const char *name);

/**
 *     Define AST Tree Node
 */
struct ASTNode
{
    Kind kind;             // ast node kind
    struct CType *ctype;   // ast value type (only expression valid)
    char *value;           // ast value
    struct List *children; // ast children

    struct SymbolTable *table; // symbol
    struct ASTNode *prt;       // scope chain
    void (*del)(void *);       // del AST Tree
};

struct ASTNode *new_literal(struct CType *ctype, const char *value);
struct ASTNode *new_unary(Kind kind, struct CType *ctype, struct ASTNode *operand);
struct ASTNode *new_binary(Kind kind, struct CType *ctype, struct ASTNode *left, struct ASTNode *right);
struct ASTNode *new_declare(struct CType *ctype, const char *value, struct ASTNode *initializer);
struct ASTNode *new_label(int number);
struct ASTNode *new_jump_false(struct ASTNode *condition, const char *value);
struct ASTNode *new_jump(const char *value);
struct ASTNode *new_scope(struct ASTNode *prt);

/**
 *     Define struct Parser
 */
struct Parser
{
    int number; // label number

    struct Lexer *lexer;                              // lexer
    void (*next)(struct Parser *parser);              // next token
    Token (*token)(struct Parser *parser);            // current token
    struct Span (*span)(struct Parser *parser);       // current token
    const char *(*value)(struct Parser *parser);      // current token value
    int (*match)(struct Parser *parser, Token token); // match token

    struct ASTNode *curr;                                        // current scope
    void (*append)(struct Parser *parser, struct ASTNode *data); // append statement
    void (*enter)(struct Parser *parser);                        // enter scope
    void (*leave)(struct Parser *parser);                        // leave scope
    void (*del)(struct Parser *parser);                          // del parser
};

struct Parser *new_parser(FILE *in);

/**
 *     Expression
 */
struct ASTNode *parse_primary(struct Parser *parser);
struct ASTNode *parse_prefix(struct Parser *parser, int prt_prior);
struct ASTNode *parse_suffix(struct ASTNode *left, struct Parser *parser);
struct ASTNode *parse_binary(struct ASTNode *left, struct Parser *parser, int prt_prior);
struct ASTNode *parse_expression(struct Parser *parser, int prt_prior);

/**
 *     Statement & Statements
 */
void parse_statement(struct Parser *parser);
void parse_declare(struct Parser *parser);
void parse_if(struct Parser *parser);
void parse_else(struct Parser *parser, struct ASTNode *trueEnd);
void parse_for(struct Parser *parser);
void parse_while(struct Parser *parser);
void parse_statements(struct Parser *parser);
void parse_program(struct Parser *parser);
#endif