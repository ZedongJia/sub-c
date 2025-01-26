#include "syntax/token.h"
#include <stdlib.h>
#include <string.h>

char *getTokenTypeValue(TokenType tokenType)
{
    switch (tokenType)
    {
    case ErrToken:
        return "Err";
    case IntLiteralToken:
        return "IntLiteral";
    case StringLiteralToken:
        return "String";
    // type-keywords
    case IntToken:
        return "Int";
    case CharToken:
        return "Char";
    // keywords
    case IdentifierToken:
        return "Identifier";
    case TrueToken:
        return "True";
    case FalseToken:
        return "False";
    case IfToken:
        return "If";
    case ElseToken:
        return "Else";
    case ForToken:
        return "For";
    case WhileToken:
        return "While";
    // scope
    case LeftBracket:
        return "LeftBracket";
    case RightBracket:
        return "RightBracket";
    case LeftBrace:
        return "LeftBrace";
    case RightBrace:
        return "RightBrace";
    // binary operator
    case PlusToken:
        return "Plus";
    case MinusToken:
        return "Minus";
    case StarToken:
        return "Star";
    case SlashToken:
        return "Slash";
    case EqualToken:
        return "Equal";
    // logic
    case GreaterToken:
        return "Greater";
    case GreaterEqualToken:
        return "GreaterEqual";
    case LessToken:
        return "Less";
    case LessEqualToken:
        return "LessEqual";
    case DoubleEqualToken:
        return "DoubleEqual";
    case NotEqualToken:
        return "NotEqual";
    case DoubleLogicAndToken:
        return "DoubleLogicAnd";
    case DoubleLogicOrToken:
        return "DoubleLogicOr";
    case LogicAndToken:
        return "LogicAnd";
    case LogicOrToken:
        return "LogicOr";
    case LogicNotToken:
        return "LogicNot";
    // separator
    case CommaToken:
        return "Comma";
    case SemiColon:
        return "SemiColon";
    // other
    case WhiteSpaceToken:
        return "WhiteSpace";
    case EndOfFileToken:
        return "EndOfFile";
    default:
        return "Unexpect";
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

int getUnaryTokenPriority(TokenType tokenType)
{
    switch (tokenType)
    {
    case PlusToken:
    case MinusToken:
    case LogicNotToken:
        return 9;
    default:
        return 0;
    }
}

int getBinaryTokenPriority(TokenType tokenType)
{
    switch (tokenType)
    {
    case StarToken:
    case SlashToken:
        return 8;
    case PlusToken:
    case MinusToken:
        return 7;
    case GreaterToken:
    case GreaterEqualToken:
    case LessToken:
    case LessEqualToken:
        return 6;
    case DoubleEqualToken:
    case NotEqualToken:
        return 5;
    case LogicAndToken:
    case LogicOrToken:
        return 4;
    case DoubleLogicAndToken:
    case DoubleLogicOrToken:
        return 3;
    case EqualToken:
        return 2;
    case CommaToken:
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
    case EqualToken:
        return 1;
    default:
        return 0;
    }
}

Token *createToken(TokenType tokenType, char *value, int length)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->tokenType = tokenType;
    token->value = (char *)malloc(length * sizeof(char));
    strcpy(token->value, value);
    token->length = length;
    return token;
}

Token *createSymbolToken(TokenType tokenType)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->tokenType = tokenType;
    token->value = NULL;
    token->length = 0;
    return token;
}

void freeToken(Token *token)
{
    if (token == NULL)
        return;
    if (token->value != NULL)
        free(token->value);
    token->value = NULL;
    free(token);
}