#include "syntax/lexer.h"
#include <stdlib.h>
#include <string.h>

Lexer *createLexer(FILE *file)
{
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    lexer->__postChar = '\0';
    lexer->__currChar = '\0';
    lexer->postToken = NULL;
    lexer->currToken = NULL;
    lexer->__file = file;
    return lexer;
}

void freeLexer(Lexer *lexer)
{
    if (lexer == NULL)
        return;
    freeToken(lexer->currToken);
    lexer->currToken = NULL;
    freeToken(lexer->postToken);
    lexer->postToken = NULL;
    lexer->__file = NULL;
    free(lexer);
}

void __peekChar(Lexer *lexer)
{
    if (lexer->__postChar == '\0')
        lexer->__postChar = getc(lexer->__file);
}

void __nextChar(Lexer *lexer)
{
    if (lexer->__postChar != '\0')
    {
        lexer->__currChar = lexer->__postChar;
        lexer->__postChar = '\0';
    }
    else
    {
        lexer->__currChar = getc(lexer->__file);
    }
}

Token *__lexNumber(Lexer *lexer)
{
    int length = 0;
    lexer->__buffer[length] = lexer->__currChar;
    length++;
    __peekChar(lexer);
    while (isDigit(lexer->__postChar))
    {
        __nextChar(lexer);
        lexer->__buffer[length] = lexer->__currChar;
        length++;
        __peekChar(lexer);
    }
    lexer->__buffer[length] = '\0';
    length++;
    return createToken(IntLiteralToken, lexer->__buffer, length);
}

Token *__lexKeywordOrIdentifier(Lexer *lexer)
{
    int length = 0;
    lexer->__buffer[length] = lexer->__currChar;
    length++;
    __peekChar(lexer);
    while (isLetter(lexer->__postChar) || lexer->__postChar == '_' || isDigit(lexer->__postChar))
    {
        __nextChar(lexer);
        lexer->__buffer[length] = lexer->__currChar;
        length++;
        __peekChar(lexer);
    }
    lexer->__buffer[length] = '\0';
    length++;
    if (strcmp(lexer->__buffer, "int") == 0)
        return createSymbolToken(IntToken);
    if (strcmp(lexer->__buffer, "char") == 0)
        return createSymbolToken(CharToken);
    else if (strcmp(lexer->__buffer, "true") == 0)
        return createSymbolToken(TrueToken);
    else if (strcmp(lexer->__buffer, "false") == 0)
        return createSymbolToken(FalseToken);
    else if (strcmp(lexer->__buffer, "if") == 0)
        return createSymbolToken(IfToken);
    else if (strcmp(lexer->__buffer, "else") == 0)
        return createSymbolToken(ElseToken);
    else if (strcmp(lexer->__buffer, "for") == 0)
        return createSymbolToken(ForToken);
    else if (strcmp(lexer->__buffer, "while") == 0)
        return createSymbolToken(WhileToken);
    else
        return createToken(IdentifierToken, lexer->__buffer, length);
}

Token *__lexString(Lexer *lexer)
{
    int length = 0;
    lexer->__buffer[length] = lexer->__currChar;
    length++;
    int isDone = 0;
    while (!isDone)
    {
        __peekChar(lexer);
        switch (lexer->__postChar)
        {
        case '\\': {
            __nextChar(lexer);
            lexer->__buffer[length] = lexer->__currChar;
            length++;
            break;
        }
        case '\"': {
            isDone = 1;
            break;
        }
        case '\n':
        case -1: {
            // err
            printf("\033[35mError: unclosed string\033[0m\n");
            isDone = 1;
            break;
        }
        }
        __nextChar(lexer);
        lexer->__buffer[length] = lexer->__currChar;
        length++;
    }
    lexer->__buffer[length] = '\0';
    length++;
    return createToken(StringLiteralToken, lexer->__buffer, length);
}

Token *__lex(Lexer *lexer)
{
    __nextChar(lexer);
    if (isDigit(lexer->__currChar))
        return __lexNumber(lexer);
    if (isLetter(lexer->__currChar) || lexer->__currChar == '_')
        return __lexKeywordOrIdentifier(lexer);
    switch (lexer->__currChar)
    {
    case '\"':
        return __lexString(lexer);
    case '+':
        return createSymbolToken(PlusToken);
    case '-':
        return createSymbolToken(MinusToken);
    case '*':
        return createSymbolToken(StarToken);
    case '/':
        return createSymbolToken(SlashToken);
    case '>': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            return createSymbolToken(GreaterEqualToken);
        }
        else
            return createSymbolToken(GreaterToken);
    }
    case '<': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            return createSymbolToken(LessEqualToken);
        }
        else
            return createSymbolToken(LessToken);
    }
    case '=': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            return createSymbolToken(DoubleEqualToken);
        }
        else
            return createSymbolToken(EqualToken);
    }
    case '&': {
        __peekChar(lexer);
        if (lexer->__postChar == '&')
        {
            __nextChar(lexer);
            return createSymbolToken(DoubleLogicAndToken);
        }
        else
            return createSymbolToken(LogicAndToken);
    }
    case '|': {
        __peekChar(lexer);
        if (lexer->__postChar == '|')
        {
            __nextChar(lexer);
            return createSymbolToken(DoubleLogicOrToken);
        }
        else
            return createSymbolToken(LogicOrToken);
    }
    case '!': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            return createSymbolToken(NotEqualToken);
        }
        else
            return createSymbolToken(LogicNotToken);
    }
    case '(':
        return createSymbolToken(LeftParenthesis);
    case ')':
        return createSymbolToken(RightParenthesis);
    case '[':
        return createSymbolToken(LeftBracket);
    case ']':
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
    case '\n':
    case '\r':
    case '\t':
        return __lex(lexer);
    case -1:
        return createSymbolToken(EndOfFileToken);
    default:
        // error skip
        printf("\033[35mError: unexpected token %c\033[0m\n", lexer->__currChar);
        return createSymbolToken(UnexpectedToken);
    }
}

void peekToken(Lexer *lexer)
{
    if (lexer->postToken == NULL)
        lexer->postToken = __lex(lexer);
}

void nextToken(Lexer *lexer)
{
    freeToken(lexer->currToken);
    lexer->currToken = NULL;
    if (lexer->postToken != NULL)
    {
        lexer->currToken = lexer->postToken;
        lexer->postToken = NULL;
    }
    else
    {
        lexer->currToken = __lex(lexer);
    }
}

void matchToken(Lexer *lexer, TokenType expectedType)
{
    peekToken(lexer);
    if (lexer->postToken->tokenType == expectedType)
    {
        nextToken(lexer);
    }
    else
    {
        printf("\033[35mError: unexpected %s, expect %s\033[0m\n", getTokenTypeValue(lexer->postToken->tokenType),
               getTokenTypeValue(expectedType));
    }
}
