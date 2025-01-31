#include "syntax/lexer.h"
#include <stdlib.h>
#include <string.h>

Lexer *createLexer(FILE *file)
{
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    lexer->__file = file;
    lexer->__postChar = '\0';
    lexer->__currChar = '\0';
    lexer->postToken = NULL;
    lexer->currToken = NULL;
    lexer->line = 1;
    lexer->column = 0;
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
    lexer->column++;
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
    return createToken(INT_LITERAL_TOKEN, lexer->__buffer, length, lexer->line, lexer->column - length + 1);
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
    TokenType type;
    if (strcmp(lexer->__buffer, "int") == 0)
        type = INT_TOKEN;
    else if (strcmp(lexer->__buffer, "char") == 0)
        type = CHAR_TOKEN;
    else if (strcmp(lexer->__buffer, "if") == 0)
        type = IF_TOKEN;
    else if (strcmp(lexer->__buffer, "else") == 0)
        type = ELSE_TOKEN;
    else if (strcmp(lexer->__buffer, "for") == 0)
        type = FOR_TOKEN;
    else if (strcmp(lexer->__buffer, "while") == 0)
        type = WHILE_TOKEN;
    else if (strcmp(lexer->__buffer, "true") == 0)
        type = TRUE_TOKEN;
    else if (strcmp(lexer->__buffer, "false") == 0)
        type = FALSE_TOKEN;
    else
        type = IDENTIFIER_TOKEN;
    return createToken(type, lexer->__buffer, length, lexer->line, lexer->column - length + 1);
}

Token *__lexString(Lexer *lexer)
{
    int length = 0;
    lexer->__buffer[length] = lexer->__currChar;
    length++;
    int isDone = 0, isError = 0;
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
            reportUnclosedString(lexer->line, lexer->column - length + 1);
            isError = 1;
            break;
        }
        }
        if (isError)
            break;
        __nextChar(lexer);
        lexer->__buffer[length] = lexer->__currChar;
        length++;
    }
    lexer->__buffer[length] = '\0';
    length++;
    return createToken(STRING_LITERAL_TOKEN, lexer->__buffer, length, lexer->line, lexer->column - length + 1);
}

Token *__lex(Lexer *lexer)
{
    __nextChar(lexer);
    if (isDigit(lexer->__currChar))
        return __lexNumber(lexer);
    if (isLetter(lexer->__currChar) || lexer->__currChar == '_')
        return __lexKeywordOrIdentifier(lexer);
    TokenType type;
    switch (lexer->__currChar)
    {
    case '\"':
        return __lexString(lexer);
    case '+':
        type = PLUS_TOKEN;
        break;
    case '-':
        type = MINUS_TOKEN;
        break;
    case '*':
        type = STAR_TOKEN;
        break;
    case '/':
        type = SLASH_TOKEN;
        break;
    case '>': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            type = GREATER_EQUAL_TOKEN;
        }
        else
            type = GREATER_TOKEN;
        break;
    }
    case '<': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            type = LESS_EQUAL_TOKEN;
        }
        else
            type = LESS_TOKEN;
        break;
    }
    case '=': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            type = DOUBLE_EQUAL_TOKEN;
        }
        else
            type = EQUAL_TOKEN;
        break;
    }
    case '&': {
        __peekChar(lexer);
        if (lexer->__postChar == '&')
        {
            __nextChar(lexer);
            type = DOUBLE_LOGIC_AND_TOKEN;
        }
        else
            type = LOGIC_AND_TOKEN;
        break;
    }
    case '|': {
        __peekChar(lexer);
        if (lexer->__postChar == '|')
        {
            __nextChar(lexer);
            type = DOUBLE_LOGIC_OR_TOKEN;
        }
        else
            type = LOGIC_OR_TOKEN;
        break;
    }
    case '!': {
        __peekChar(lexer);
        if (lexer->__postChar == '=')
        {
            __nextChar(lexer);
            type = NOT_EQUAL_TOKEN;
        }
        else
            type = LOGIC_NOT_TOKEN;
        break;
    }
    case '(':
        type = LEFT_PARENTHESIS;
        break;
    case ')':
        type = RIGHT_PARENTHESIS;
        break;
    case '[':
        type = LEFT_BRACKET;
        break;
    case ']':
        type = RIGHT_BRACKET;
        break;
    case '{':
        type = LEFT_BRACE;
        break;
    case '}':
        type = RIGHT_BRACE;
        break;
    case ',':
        type = COMMA_TOKEN;
        break;
    case ';':
        type = SEMI_COLON_TOKEN;
        break;
    case '\n':
        lexer->line++;
        lexer->column = 0;
        return __lex(lexer);
    case ' ':
    case '\r':
    case '\t':
        return __lex(lexer);
    case -1:
        type = END_OF_FILE_TOKEN;
        break;
    default:
        // error skip
        reportUnexpectedChar(lexer->line, lexer->column, lexer->__currChar);
        return __lex(lexer);
    }
    return createSymbolToken(type, lexer->line, lexer->column);
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

int matchToken(Lexer *lexer, TokenType expectedType)
{
    peekToken(lexer);
    TokenType type = lexer->postToken->type;
    int line = lexer->postToken->line;
    int column = lexer->postToken->column;
    if (type == expectedType)
    {
        nextToken(lexer);
        return 1;
    }
    else
    {
        reportUnexpectedToken(line, column, getTokenTypeValue(type), getTokenTypeValue(expectedType));
        switch (type)
        {
        case RIGHT_BRACE:
        case RIGHT_BRACKET:
        case RIGHT_PARENTHESIS:
        case ELSE_TOKEN:
            nextToken(lexer); // drop tokens which can't appear alone
            break;
        default:
            break;
        }
        return 0;
    }
}
