#ifndef TOKEN
#define TOKEN
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
    PLUS_T,   // \+
    MIN_T,    // \-
    STAR_T,   // \*
    SLASH_T,  // /
    EQ_T, // =
    GT_T,     // >
    GE_T,     // >=
    LT_T,     // <
    LE_T,     // <=
    D_EQ_T,     // ==
    NE_T,     // !=
    D_AND_T,  // &&
    D_OR_T,   // ||
    AND_T,    // &
    OR_T,     // |
    NOT_T,    // !
    // separator
    COMMA_T,      // ,
    SEMI_COLON_T, // ;
    // other
    EOF_T // -1
} Token;

const char *tokenName(Token token);
int isDigit(int ch);
int isLetter(int ch);
int canSkip(int ch);
#endif