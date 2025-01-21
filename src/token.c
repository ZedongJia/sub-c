#include "token.h"
#include <stdlib.h>
#include <string.h>

char *getTokenTypeValue(enum TokenType tokenType)
{
    switch (tokenType)
    {
    case ErrToken:
        return "ErrToken";
    case LiteralToken:
        return "LiteralToken";
    case LeftBracket:
        return "LeftBracket";
    case RightBracket:
        return "RightBracket";
    case PlusToken:
        return "PlusToken";
    case MinusToken:
        return "MinusToken";
    case StarToken:
        return "StarToken";
    case SlashToken:
        return "SlashToken";
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
    case WhiteSpaceToken:
        return "WhiteSpaceToken";
    case EndOfLineToken:
        return "EndOfLineToken";
    default:
        return "UnexpectToken";
    }
}
int isDigit(int ch)
{
    return ch >= '0' && ch <= '9';
}
int getUnaryTokenPriority(enum TokenType tokenType)
{
    switch (tokenType)
    {
    case PlusToken:
    case MinusToken:
    case LogicNotToken:
        return 5;
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
        return 4;
    case PlusToken:
    case MinusToken:
        return 3;
    case LogicAndToken:
    case LogicOrToken:
        return 2;
    case DoubleLogicAndToken:
    case DoubleLogicOrToken:
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