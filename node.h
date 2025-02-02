#ifndef NODE
#define NODE
#include "token.h"

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
    ADD_N,    // \+
    SUB_N,    // \-
    MUL_N,    // \*
    DIV_N,    // /
    ASSIGN_N, // =
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

const char *kindName(Kind kind);
int unaryPriority(Kind kind);
int binaryPriority(Kind kind);
/// @brief association, 0 means left, 1 means right
int assoc(Kind kind);
/// @brief to unary kind
Kind toUKind(Token token);
/// @brief to binary kind
Kind toBKind(Token token);
#endif