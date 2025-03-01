#ifndef DEFS
#define DEFS
#include "list.h"
#include <stdio.h>

/**
 *     token_t enum
 */
typedef enum token
{
    INT_LIT_T = 1,
    STR_LIT_T,
    // token-keywords
    INT_T,
    CHAR_T,
    // keywords
    ID_T,
    IF_T,
    ELSE_T,
    FOR_T,
    WHILE_T,
    CONTINUE_T,
    BREAK_T,
    // scope
    L_PAREN_T, // (
    R_PAREN_T, // )
    L_BRK_T,   // [
    R_BRK_T,   // ]
    L_BRC_T,   // {
    R_BRC_T,   // }
    // operator
    PLUS_T,  // \+
    MIN_T,   // \-
    STAR_T,  // \*
    SLASH_T, // /
    EQ_T,    // =
    GT_T,    // >
    GE_T,    // >=
    LT_T,    // <
    LE_T,    // <=
    D_EQ_T,  // ==
    NE_T,    // !=
    D_AND_T, // &&
    D_OR_T,  // ||
    AND_T,   // &
    OR_T,    // |
    NOT_T,   // !
    // separator
    COMMA_T,      // ,
    SEMI_COLON_T, // ;
    // other
    EOF_T // -1
} token_t;

const char *token_name(token_t token);

/**
 *     Syntax kind enum
 */
typedef enum syntax_kind
{
    // expression
    IMM_KIND = 1,
    LIT_KIND,
    VAR_KIND,
    // unary operator
    ADDR_KIND,    // *
    ADDR_OF_KIND, // &
    P_ADD_KIND,   // \+()
    P_SUB_KIND,   // \-()
    NOT_KIND,     // !
    // binary operator
    ASSIGN_KIND, // =
    MUL_KIND,    // \*
    DIV_KIND,    // /
    ADD_KIND,    // \+
    SUB_KIND,    // \-
    GT_KIND,     // >
    GE_KIND,     // >=
    LT_KIND,     // <
    LE_KIND,     // <=
    EQ_KIND,     // ==
    NE_KIND,     // !=
    AND_KIND,    // &&
    OR_KIND,     // ||
    L_AND_KIND,  // &
    L_OR_KIND,   // |
    // multi operator
    CALL_KIND,
    COMMA_KIND,
    // statement
    DEC_KIND,
    LABEL_KIND,
    JUMP_FALSE_KIND,
    JUMP_KIND,
    SCOPE_KIND
} syntax_kind_t;

const char *syntax_kind_name(syntax_kind_t kind);
int uprior(syntax_kind_t kind);
int bprior(syntax_kind_t kind);
/// @brief association, 0 means left, 1 means right
int assoc(syntax_kind_t kind);
/// @brief to unary kind
syntax_kind_t to_usyntax_kind(token_t token);
/// @brief to binary kind
syntax_kind_t to_bsyntax_kind(token_t token);

/**
 *     Type enum
 */
typedef enum type
{
    CHAR_TYPE = 1,
    INT_TYPE,
} type_t;

char *type_name(type_t type);
type_t to_type(token_t token);
int type_size(type_t type);

/**
 *     Define lexer
 */
typedef struct span
{
    int row;
    int col;
} span_t;

typedef struct lexer
{
    FILE *in; // lex file
    char pc;  // putback char
    char cc;  // current char
    token_t token;
    char text[255]; // buf
    int len;
    span_t span; // value span

    void (*next)(struct lexer *self);
    int (*match)(struct lexer *self, token_t what);
    void (*del)(struct lexer *self);
} lexer_t;

lexer_t *new_lexer(FILE *in);

/**
 *     Define complex type
 */
typedef struct ctype
{
    type_t type;
    int offset[16]; // max type recursive = 16
    int ptr;
    int mut;
    struct ctype *(*clone)(const struct ctype *self);
    void (*del)(struct ctype *self);
} ctype_t;

ctype_t *new_ctype(type_t type, int mut);
int ctype_offset(const ctype_t *ctype);
int ctype_aligned_offset(const ctype_t *ctype);
int ctype_list_aligned_offset(const list_t *list);
void point_to(ctype_t *ctype);
void content_of(ctype_t *ctype);
void array_of(ctype_t *ctype, int size);
ctype_t *type_widden(ctype_t *left, ctype_t *right);
/// @brief check unary valid, if invalid return `NULL`
ctype_t *unary_compatible(syntax_kind_t kind, ctype_t *left);
/// @brief check binary valid, if invalid return `NULL`
ctype_t *binary_compatible(syntax_kind_t kind, ctype_t *left, ctype_t *right);

/**
 *     Define variable symbol
 */
typedef struct symbol
{
    const ctype_t *ctype;       // symbol type
    const char *name;           // symbol name
    const list_t *params_ctype; // params type
    union {
        int label;  // symbol label (for const)
        int offset; // offset from %ebp
    };
} symbol_t;

typedef struct symbol_table
{
    list_t *symbols; // symbols
    int offset;      // declare offset
    int call_offset; // function call offset
    const symbol_t *(*look_up)(struct symbol_table *self, const char *name);
    const symbol_t *(*try_declare)(struct symbol_table *self, const ctype_t *ctype, const char *name,
                                   const list_t *params_ctype);
    int (*declare)(struct symbol_table *self, const ctype_t *ctype, const char *name, const list_t *params_ctype);
    void (*del)(struct symbol_table *self);
    struct symbol_table *prt; // parent symbol table
} symbol_table_t;

symbol_table_t *new_symbol_table(symbol_table_t *prt);

/**
 *     Define AST Tree
 */
typedef struct ast
{
    syntax_kind_t kind;                                // ast syntax kind
    const symbol_t *symbol;                            // ast value symbol
    ctype_t *ctype;                                    // ast value type (only expression valid)
    char *value;                                       // ast value
    list_t *children;                                  // ast children
    void (*append)(struct ast *self, struct ast *ast); // append AST Tree
    list_iterator_t *(*get_iter)(struct ast *self);    // get iterator

    symbol_table_t *var_table; // variable table
    struct ast *prt;           // scope chain
    const struct ast *begin;   // scope ref begin
    const struct ast *end;     // scope ref end
    void (*del)(void *ast);    // del AST Tree
} ast_t;

ast_t *new_imm(ctype_t *ctype, const char *value);
ast_t *new_literal(ctype_t *ctype, const symbol_t *symbol, const char *value);
ast_t *new_var(ctype_t *ctype, const symbol_t *symbol, const char *name);
ast_t *new_unary(syntax_kind_t kind, ctype_t *ctype, ast_t *operand);
ast_t *new_binary(syntax_kind_t kind, ctype_t *ctype, ast_t *left, ast_t *right);
ast_t *new_declare(ctype_t *ctype, const char *value, ast_t *initializer);
ast_t *new_label(int number);
ast_t *new_jump_false(ast_t *condition, const char *value);
ast_t *new_jump(const char *value);
ast_t *new_scope(ast_t *prt);

/**
 *     Parse Main
 */
ast_t *parse_primary();
ast_t *parse_prefix(int prt_prior);
ast_t *parse_suffix(ast_t *left);
ast_t *parse_binary(ast_t *left, int prt_prior);
ast_t *parse_expr(int prt_prior);
void parse_enter();
void parse_leave();
void parse_stmt();
void parse_pointer_ctype(ctype_t *ctype);
void parse_array_ctype(ctype_t *ctype);
void parse_declare();
void parse_if();
void parse_else(ast_t *true_end);
void parse_for();
void parse_while();
void parse_statements();
void parse_program();

/**
 *     Code generate
 */
int alloc_reg();
const char *reg_name(int reg);
void free_reg(int reg);
void gen_text();
void gen_label(const char *name);
void gen_movl(const char *left, const char *right);
void gen_addl(const char *left, const char *right);
void gen_subl(const char *left, const char *right);
void gen_imull(const char *left);
void gen_idivl(const char *left);
void gen_pushl(const char *left);
void gen_popl(const char *left);
void gen_program();

/**
 *     Debug
 */
void debug_pretty_tree(ast_t *ast);

/**
 *     Error
 */
void err_unclosed_str(span_t *span);
void err_unexpect_char(span_t *span, const char c);
void err_unexpect_token(span_t *span, const token_t token);
void err_var_undefined(span_t *span, const char *var);
void err_var_defined(span_t *span, const char *var);
void err_incompat_binary(span_t *span, ctype_t *left, syntax_kind_t op, ctype_t *right);
void err_incompat_unary(span_t *span, syntax_kind_t op, ctype_t *operand);
void err_continus(span_t *span);
void err_break(span_t *span);
#endif