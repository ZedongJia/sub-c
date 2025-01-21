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
    if (ch == '(')
        return createSymbolToken(LeftBracket);
    if (ch == ')')
        return createSymbolToken(RightBracket);
    if (ch == '+')
        return createSymbolToken(PlusToken);
    if (ch == '-')
        return createSymbolToken(MinusToken);
    if (ch == '*')
        return createSymbolToken(StarToken);
    if (ch == '/')
        return createSymbolToken(SlashToken);
    if (ch == '&')
    {
        char postCh = peekChar(lexer);
        if (postCh == '&')
        {
            nextChar(lexer);
            return createSymbolToken(DoubleLogicAndToken);
        }
        else
            return createSymbolToken(LogicAndToken);
    }
    if (ch == '|')
    {
        char postCh = peekChar(lexer);
        if (postCh == '|')
        {
            nextChar(lexer);
            return createSymbolToken(DoubleLogicOrToken);
        }
        else
            return createSymbolToken(LogicOrToken);
    }
    if (ch == '!')
        return createSymbolToken(LogicNotToken);
    if (ch == ' ')
        return nextToken(lexer);
    if (ch == '\n')
        return createSymbolToken(EndOfLineToken);
    // error skip
    printf("\033[35mError: unexpected token %c\033[0m\n", ch);
    return createSymbolToken(ErrToken);
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
