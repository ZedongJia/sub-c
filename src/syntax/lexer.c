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
    return createToken(INT_LITERAL_TOKEN, lexer->__buffer, length);
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
        return createSymbolToken(INT_TOKEN);
    if (strcmp(lexer->__buffer, "char") == 0)
        return createSymbolToken(CHAR_TOKEN);
    else if (strcmp(lexer->__buffer, "true") == 0)
        return createSymbolToken(TRUE_TOKEN);
    else if (strcmp(lexer->__buffer, "false") == 0)
        return createSymbolToken(FALSE_TOKEN);
    else if (strcmp(lexer->__buffer, "if") == 0)
        return createSymbolToken(IF_TOKEN);
    else if (strcmp(lexer->__buffer, "else") == 0)
        return createSymbolToken(ELSE_TOKEN);
    else if (strcmp(lexer->__buffer, "for") == 0)
        return createSymbolToken(FOR_TOKEN);
    else if (strcmp(lexer->__buffer, "while") == 0)
        return createSymbolToken(WHILE_TOKEN);
    else
        return createToken(IDENTIFIER_TOKEN, lexer->__buffer, length);
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
    return createToken(STRING_LITERAL_TOKEN, lexer->__buffer, length);
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
        return createSymbolToken(PLUS_TOKEN);
    case '-':
        return createSymbolToken(MINUS_TOKEN);
    case '*':
        return createSymbolToken(STAR_TOKEN);
    case '/':
        return createSymbolToken(SLASH_TOKEN);
    case '>': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            return createSymbolToken(GREATER_EQUAL_TOKEN);
        }
        else
            return createSymbolToken(GREATER_TOKEN);
    }
    case '<': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            return createSymbolToken(LESS_EQUAL_TOKEN);
        }
        else
            return createSymbolToken(LESS_TOKEN);
    }
    case '=': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            return createSymbolToken(DOUBLE_EQUAL_TOKEN);
        }
        else
            return createSymbolToken(EQUAL_TOKEN);
    }
    case '&': {
        __peekChar(lexer);
        if (lexer->__postChar == '&')
        {
            __nextChar(lexer);
            return createSymbolToken(DOUBLE_LOGIC_AND_TOKEN);
        }
        else
            return createSymbolToken(LOGIC_AND_TOKEN);
    }
    case '|': {
        __peekChar(lexer);
        if (lexer->__postChar == '|')
        {
            __nextChar(lexer);
            return createSymbolToken(DOUBLE_LOGIC_OR_TOKEN);
        }
        else
            return createSymbolToken(LOGIC_OR_TOKEN);
    }
    case '!': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            return createSymbolToken(NOT_EQUAL_TOKEN);
        }
        else
            return createSymbolToken(LOGIC_NOT_TOKEN);
    }
    case '(':
        return createSymbolToken(LEFT_PARENTHESIS);
    case ')':
        return createSymbolToken(RIGHT_PARENTHESIS);
    case '[':
        return createSymbolToken(LEFT_BRACKET);
    case ']':
        return createSymbolToken(RIGHT_BRACKET);
    case '{':
        return createSymbolToken(LEFT_BRACE);
    case '}':
        return createSymbolToken(RIGHT_BRACE);
    case ',':
        return createSymbolToken(COMMA_TOKEN);
    case ';':
        return createSymbolToken(SEMI_COLON_TOKEN);
    case ' ':
    case '\n':
    case '\r':
    case '\t':
        return __lex(lexer);
    case -1:
        return createSymbolToken(END_OF_FILE_TOKEN);
    default:
        // error skip
        printf("\033[35mError: unexpected token %c\033[0m\n", lexer->__currChar);
        return createSymbolToken(UNEXPECTED_TOKEN);
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
