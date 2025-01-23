#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Lexer *createLexer()
{
    struct Lexer *lexer = (struct Lexer *)malloc(sizeof(struct Lexer));
    lexer->peekChar = '\0';
    lexer->currentChar = '\0';
    lexer->peekToken = NULL;
    lexer->currentToken = NULL;
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
    if (lexer->currentToken != NULL)
    {
        freeToken(lexer->currentToken);
        lexer->currentToken = NULL;
    }
    if (lexer->peekToken != NULL)
    {
        freeToken(lexer->peekToken);
        lexer->peekToken = NULL;
    }
}
const char peekChar(struct Lexer *lexer)
{
    if (lexer->peekChar == '\0')
    {
        lexer->peekChar = getchar();
    }
    return lexer->peekChar;
}
const char nextChar(struct Lexer *lexer)
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
struct Token *lex(struct Lexer *lexer)
{
    char ch = nextChar(lexer);
    if (isDigit(ch))
    {
        int length = 0;
        lexer->buffer[length] = ch;
        length++;
        ch = peekChar(lexer);
        // parse digits
        while (isDigit(ch))
        {
            nextChar(lexer);
            lexer->buffer[length] = ch;
            length++;
            ch = peekChar(lexer);
        }
        lexer->buffer[length] = '\0';
        length++;
        return createToken(IntLiteralToken, lexer->buffer, length);
    }
    if (isLetter(ch) || ch == '_')
    {
        int length = 0;
        lexer->buffer[length] = ch;
        length++;
        ch = peekChar(lexer);
        // parse text
        while (isLetter(ch) || ch == '_')
        {
            nextChar(lexer);
            lexer->buffer[length] = ch;
            length++;
            ch = peekChar(lexer);
        }
        lexer->buffer[length] = '\0';
        length++;
        if (strcmp(lexer->buffer, "true") == 0)
            return createSymbolToken(TrueToken);
        else if (strcmp(lexer->buffer, "false") == 0)
            return createSymbolToken(FalseToken);
        else if (strcmp(lexer->buffer, "int") == 0)
            return createSymbolToken(IntToken);
        else
            return createToken(IdentifierToken, lexer->buffer, length);
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
    case '{':
        return createSymbolToken(LeftBrace);
    case '}':
        return createSymbolToken(RightBrace);
    case ',':
        return createSymbolToken(CommaToken);
    case ';':
        return createSymbolToken(SemiColon);
    case ' ':
        return lex(lexer);
    case '\n':
        return createSymbolToken(EndOfFileToken);
    default:
        // error skip
        printf("\033[35mError: unexpected token %c\033[0m\n", ch);
        return createSymbolToken(ErrToken);
    }
}
const struct Token *peekToken(struct Lexer *lexer)
{
    if (lexer->peekToken == NULL)
    {
        // peek next Token
        lexer->peekToken = lex(lexer);
    }
    return lexer->peekToken;
}
const struct Token *nextToken(struct Lexer *lexer)
{
    if (lexer->currentToken != NULL)
    {
        // clear currentToken
        freeToken(lexer->currentToken);
        lexer->currentToken = NULL;
    }
    if (lexer->peekToken != NULL)
    {
        // load from peekToken
        lexer->currentToken = lexer->peekToken;
        lexer->peekToken = NULL;
    }
    else
    {
        // load new Token
        lexer->currentToken = lex(lexer);
    }
    return lexer->currentToken;
}