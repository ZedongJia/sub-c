#include "token.h"
#include <stdlib.h>
#include <string.h>

const char *tokenName(Token token)
{
    switch (token)
    {
    case INT_LIT_T:
        return "Int Literal";
    case STR_LIT_T:
        return "String Literal";
    // token-keywords
    case INT_T:
        return "Int";
    case CHAR_T:
        return "Char";
    // keywords
    case ID_T:
        return "Identifier";
    case IF_T:
        return "If";
    case ELSE_T:
        return "Else";
    case FOR_T:
        return "For";
    case WHILE_T:
        return "While";
    // scope
    case L_PAREN_T:
        return "(";
    case R_PAREN_T:
        return ")";
    case L_BRK_T:
        return "[";
    case R_BRK_T:
        return "]";
    case L_BRC_T:
        return "{";
    case R_BRC_T:
        return "}";
    // operator
    case PLUS_T:
        return "+";
    case MIN_T:
        return "-";
    case STAR_T:
        return "*";
    case SLASH_T:
        return "/";
    case EQ_T:
        return "=";
    case GT_T:
        return ">";
    case GE_T:
        return ">=";
    case LT_T:
        return "<";
    case LE_T:
        return "<=";
    case D_EQ_T:
        return "==";
    case NE_T:
        return "!=";
    case D_AND_T:
        return "&&";
    case D_OR_T:
        return "||";
    case AND_T:
        return "&";
    case OR_T:
        return "|";
    case NOT_T:
        return "!";
    // separator
    case COMMA_T:
        return ",";
    case SEMI_COLON_T:
        return ";";
    // other
    case EOF_T:
        return "EOF";
    default:
        return "?";
    }
}

int isDigit(int ch)
{
    return ch >= '0' && ch <= '9';
}

int isLetter(int ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

int canSkip(int ch)
{
    return ch == ' ' || ch == '\r' || ch == '\t';
}
