#ifndef ENUMS
#define ENUMS
/**
 *     Token enum
 */
typedef enum Token
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
} Token;

const char *token_name(Token token);

/**
 *     Syntax kind enum
 */
typedef enum Kind
{
    // expression
    LIT_N = 1,
    // unary operator
    ADDR_N,    // *
    ADDR_OF_N, // &
    P_ADD_N,   // \+()
    P_SUB_N,   // \-()
    NOT_N,     // !
    // binary operator
    ASSIGN_N, // =
    MUL_N,    // \*
    DIV_N,    // /
    ADD_N,    // \+
    SUB_N,    // \-
    GT_N,     // >
    GE_N,     // >=
    LT_N,     // <
    LE_N,     // <=
    EQ_N,     // ==
    NE_N,     // !=
    AND_N,    // &&
    OR_N,     // ||
    L_AND_N,  // &
    L_OR_N,   // |
    // multi operator
    CALL_N,
    COMMA_N,
    // statement
    DEC_N,
    LABEL_N,
    JUMP_FALSE_N,
    JUMP_N,
    SCOPE_N
} Kind;

const char *kind_name(Kind kind);
int uprior(Kind kind);
int bprior(Kind kind);
/// @brief association, 0 means left, 1 means right
int assoc(Kind kind);
/// @brief to unary kind
Kind to_ukind(Token token);
/// @brief to binary kind
Kind to_bkind(Token token);

/**
 *     Type enum
 */
typedef enum Type
{
    CHAR_TYPE = 1,
    INT_TYPE,
} Type;

char *type_name(Type type);
Type to_type(Token token);
int type_size(Type type);
#endif