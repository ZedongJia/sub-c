#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

struct Lexer *createLexer()
{
    struct Lexer *lexer = (struct Lexer *)malloc(sizeof(struct Lexer));
    lexer->peekChar = '\0';
    lexer->currentChar = '\0';
    return lexer;
}
void freeLexer(struct Lexer *lexer)
{
    free(lexer);
}
void clearLexer(struct Lexer *lexer)
{
    lexer->peekChar = '\0';
    lexer->currentChar = '\0';
}
struct Token *nextToken(struct Lexer *lexer)
{
    char ch = nextChar(lexer);
    if (isDigit(ch))
    {
        int bufLength = 0;
        lexer->text[bufLength] = ch;
        bufLength++;
        ch = peekChar(lexer);
        // parse digits
        while (isDigit(ch))
        {
            nextChar(lexer);
            lexer->text[bufLength] = ch;
            bufLength++;
            ch = peekChar(lexer);
        }
        lexer->text[bufLength] = '\0';
        bufLength++;
        return createToken(LiteralToken, lexer->text, bufLength);
    }
    switch (ch)
    {
    case '+':
        return createSymbolToken(PlusToken);
    case '-':
        return createSymbolToken(MinusToken);
    case '*':
        return createSymbolToken(StarToken);
    case '/':
        return createSymbolToken(SlashToken);
    case '>': {
        char postChar = peekChar(lexer);
        if (postChar == '=')
        {
            nextChar(lexer);
            return createSymbolToken(GreaterEqualToken);
        }
        else
            return createSymbolToken(GreaterToken);
    }
    case '<': {
        char postChar = peekChar(lexer);
        if (postChar == '=')
        {
            nextChar(lexer);
            return createSymbolToken(LessEqualToken);
        }
        else
            return createSymbolToken(LessToken);
    }
    case '=': {
        char postChar = peekChar(lexer);
        if (postChar == '=')
        {
            nextChar(lexer);
            return createSymbolToken(DoubleEqualToken);
        }
        else
            return createSymbolToken(EqualToken);
    }
    case '&': {
        char postChar = peekChar(lexer);
        if (postChar == '&')
        {
            nextChar(lexer);
            return createSymbolToken(DoubleLogicAndToken);
        }
        else
            return createSymbolToken(LogicAndToken);
    }
    case '|': {
        char postChar = peekChar(lexer);
        if (postChar == '|')
        {
            nextChar(lexer);
            return createSymbolToken(DoubleLogicOrToken);
        }
        else
            return createSymbolToken(LogicOrToken);
    }
    case '!': {

        char postChar = peekChar(lexer);
        if (postChar == '=')
        {
            nextChar(lexer);
            return createSymbolToken(NotEqualToken);
        }
        else
            return createSymbolToken(LogicNotToken);
    }
    case '(':
        return createSymbolToken(LeftBracket);
    case ')':
        return createSymbolToken(RightBracket);
    case ' ':
        return nextToken(lexer);
    case '\n':
        return createSymbolToken(EndOfLineToken);
    default:
        // error skip
        printf("\033[35mError: unexpected token %c\033[0m\n", ch);
        return createSymbolToken(ErrToken);
    }
}
char nextChar(struct Lexer *lexer)
{
    if (lexer->peekChar != '\0')
    {
        lexer->currentChar = lexer->peekChar;
        lexer->peekChar = '\0';
    }
    else
    {
        lexer->currentChar = getchar();
    }
    return lexer->currentChar;
}

char peekChar(struct Lexer *lexer)
{
    if (lexer->peekChar == '\0')
    {
        lexer->peekChar = getchar();
    }
    return lexer->peekChar;
}
