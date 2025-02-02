#include "token.h"
#include <stdlib.h>
#include <string.h>

const char *getTokenTypeValue(TokenType tokenType)
{
    switch (tokenType)
    {
    case INT_LITERAL_TOKEN:
        return "Int Literal";
    case STRING_LITERAL_TOKEN:
        return "String Literal";
    // tokenType-keywords
    case INT_TOKEN:
        return "Int";
    case CHAR_TOKEN:
        return "Char";
    // keywords
    case IDENTIFIER_TOKEN:
        return "Identifier";
    case TRUE_TOKEN:
        return "True";
    case FALSE_TOKEN:
        return "False";
    case IF_TOKEN:
        return "If";
    case ELSE_TOKEN:
        return "Else";
    case FOR_TOKEN:
        return "For";
    case WHILE_TOKEN:
        return "While";
    // scope
    case LEFT_PARENTHESIS:
        return "(";
    case RIGHT_PARENTHESIS:
        return ")";
    case LEFT_BRACKET:
        return "[";
    case RIGHT_BRACKET:
        return "]";
    case LEFT_BRACE:
        return "{";
    case RIGHT_BRACE:
        return "}";
    // operator
    case PLUS_TOKEN:
        return "+";
    case MINUS_TOKEN:
        return "-";
    case STAR_TOKEN:
        return "*";
    case SLASH_TOKEN:
        return "/";
    case EQUAL_TOKEN:
        return "=";
    // operate
    case DIM_TOKEN:
        return "dim";
    case CALL_TOKEN:
        return "call";
    // logic
    case GREATER_TOKEN:
        return ">";
    case GREATER_EQUAL_TOKEN:
        return ">=";
    case LESS_TOKEN:
        return "<";
    case LESS_EQUAL_TOKEN:
        return "<=";
    case DOUBLE_EQUAL_TOKEN:
        return "==";
    case NOT_EQUAL_TOKEN:
        return "!=";
    case DOUBLE_LOGIC_AND_TOKEN:
        return "&&";
    case DOUBLE_LOGIC_OR_TOKEN:
        return "||";
    case LOGIC_AND_TOKEN:
        return "&";
    case LOGIC_OR_TOKEN:
        return "|";
    case LOGIC_NOT_TOKEN:
        return "!";
    // separator
    case COMMA_TOKEN:
        return ",";
    case SEMI_COLON_TOKEN:
        return ";";
    // other
    case END_OF_FILE_TOKEN:
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

int getUnaryTokenPriority(TokenType tokenType)
{
    switch (tokenType)
    {
    case PLUS_TOKEN:
    case MINUS_TOKEN:
    case LOGIC_NOT_TOKEN:
    case STAR_TOKEN:
    case LOGIC_AND_TOKEN:
        return 9;
    default:
        return 0;
    }
}

int getBinaryTokenPriority(TokenType tokenType)
{
    switch (tokenType)
    {
    case STAR_TOKEN:
    case SLASH_TOKEN:
        return 8;
    case PLUS_TOKEN:
    case MINUS_TOKEN:
        return 7;
    case GREATER_TOKEN:
    case GREATER_EQUAL_TOKEN:
    case LESS_TOKEN:
    case LESS_EQUAL_TOKEN:
        return 6;
    case DOUBLE_EQUAL_TOKEN:
    case NOT_EQUAL_TOKEN:
        return 5;
    case LOGIC_AND_TOKEN:
    case LOGIC_OR_TOKEN:
        return 4;
    case DOUBLE_LOGIC_AND_TOKEN:
    case DOUBLE_LOGIC_OR_TOKEN:
        return 3;
    case EQUAL_TOKEN:
        return 2;
    case COMMA_TOKEN:
        return 1;
    default:
        return 0;
    }
}

int getAssociation(TokenType tokenType)
{
    // 0 means left, 1 means right
    switch (tokenType)
    {
    case EQUAL_TOKEN:
        return 1;
    default:
        return 0;
    }
}
