#include "token.h"
#include <stdlib.h>
#include <string.h>

char *getTokenTypeValue(enum TokenType tokenType)
{
    switch (tokenType)
    {
    case ErrToken:
        return "ErrToken";
    case IntLiteralToken:
        return "IntLiteralToken";
    case IdentifierToken:
        return "IdentifierToken";
    case TrueToken:
        return "TrueToken";
    case FalseToken:
        return "FalseToken";
    case LeftBracket:
        return "LeftBracket";
    case RightBracket:
        return "RightBracket";
    case LeftBrace:
        return "LeftBrace";
    case RightBrace:
        return "RightBrace";
    case PlusToken:
        return "PlusToken";
    case MinusToken:
        return "MinusToken";
    case StarToken:
        return "StarToken";
    case SlashToken:
        return "SlashToken";
    case EqualToken:
        return "EqualToken";
    case GreaterToken:
        return "GreaterToken";
    case GreaterEqualToken:
        return "GreaterEqualToken";
    case LessToken:
        return "LessToken";
    case LessEqualToken:
        return "LessEqualToken";
    case DoubleEqualToken:
        return "DoubleEqualToken";
    case NotEqualToken:
        return "NotEqualToken";
    case DoubleLogicAndToken:
        return "DoubleLogicAndToken";
    case DoubleLogicOrToken:
        return "DoubleLogicOrToken";
    case LogicAndToken:
        return "LogicAndToken";
    case LogicOrToken:
        return "LogicOrToken";
    case LogicNotToken:
        return "LogicNotToken";
    case CommaToken:
        return "CommaToken";
    case SemiColon:
        return "SemiColon";
    case WhiteSpaceToken:
        return "WhiteSpaceToken";
    case EndOfFileToken:
        return "EndOfFileToken";
    default:
        return "UnexpectToken";
    }
}
int isDigit(int ch)
{
    return ch >= '0' && ch <= '9';
}
int isLetter(int ch)
{
    return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z';
}
int getUnaryTokenPriority(enum TokenType tokenType)
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
int getBinaryTokenPriority(enum TokenType tokenType)
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
struct Token *createToken(enum TokenType tokenType, char *value, int valueLength)
{
    struct Token *token = (struct Token *)malloc(sizeof(struct Token));
    token->tokenType = tokenType;
    token->value = (char *)malloc(valueLength * sizeof(char));
    strcpy(token->value, value);
    token->valueLength = valueLength;
    return token;
}
struct Token *createSymbolToken(enum TokenType tokenType)
{
    struct Token *token = (struct Token *)malloc(sizeof(struct Token));
    token->tokenType = tokenType;
    token->value = NULL;
    token->valueLength = 0;
    return token;
}
void freeToken(struct Token *token)
{
    if (token->value != NULL)
        free(token->value);
    token->value = NULL;
    free(token);
}