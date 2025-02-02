#include "lexer.h"
#include <stdlib.h>
#include <string.h>

void __putback(Lexer *lexer)
{
    lexer->__pc = lexer->__cc;
}

void __next(Lexer *lexer)
{
    lexer->start++;
    if (lexer->__pc != '\0')
    {
        lexer->__cc = lexer->__pc;
        lexer->__pc = '\0';
    }
    else
    {
        lexer->__cc = getc(lexer->__in);
    }
}

void __lexNumber(Lexer *lexer)
{
    lexer->buf[lexer->len] = lexer->__cc;
    lexer->len++;
    __next(lexer);
    while (isDigit(lexer->__cc))
    {
        lexer->buf[lexer->len] = lexer->__cc;
        lexer->len++;
        __next(lexer);
    }
    __putback(lexer);
    lexer->buf[lexer->len] = '\0';
    lexer->len++;
    lexer->token = INT_LITERAL_TOKEN;
}

void __lexKeywordOrIdentifier(Lexer *lexer)
{
    lexer->buf[lexer->len] = lexer->__cc;
    lexer->len++;
    __next(lexer);
    while (isLetter(lexer->__cc) || lexer->__cc == '_' || isDigit(lexer->__cc))
    {
        lexer->buf[lexer->len] = lexer->__cc;
        lexer->len++;
        __next(lexer);
    }
    __putback(lexer);
    lexer->buf[lexer->len] = '\0';
    lexer->len++;
    if (strcmp(lexer->buf, "int") == 0)
        lexer->token = INT_TOKEN;
    else if (strcmp(lexer->buf, "char") == 0)
        lexer->token = CHAR_TOKEN;
    else if (strcmp(lexer->buf, "if") == 0)
        lexer->token = IF_TOKEN;
    else if (strcmp(lexer->buf, "else") == 0)
        lexer->token = ELSE_TOKEN;
    else if (strcmp(lexer->buf, "for") == 0)
        lexer->token = FOR_TOKEN;
    else if (strcmp(lexer->buf, "while") == 0)
        lexer->token = WHILE_TOKEN;
    else if (strcmp(lexer->buf, "true") == 0)
        lexer->token = TRUE_TOKEN;
    else if (strcmp(lexer->buf, "false") == 0)
        lexer->token = FALSE_TOKEN;
    else
        lexer->token = IDENTIFIER_TOKEN;
}

void __lexString(Lexer *lexer)
{
    lexer->buf[lexer->len] = lexer->__cc;
    lexer->len++;
    __next(lexer);
    int isDone = 0, isError = 0;
    while (!isDone)
    {
        switch (lexer->__cc)
        {
        case '\\': {
            lexer->buf[lexer->len] = lexer->__cc;
            lexer->len++;
            __next(lexer);
            break;
        }
        case '\"': {
            isDone = 1;
            break;
        }
        case '\n':
        case -1: {
            // err
            reportUnclosedString(lexer->line, lexer->start);
            isError = 1;
            break;
        }
        }
        if (isError)
            break;
        lexer->buf[lexer->len] = lexer->__cc;
        lexer->len++;
        __next(lexer);
    }
    __putback(lexer);
    lexer->buf[lexer->len] = lexer->__cc;
    lexer->len++;
    lexer->token = STRING_LITERAL_TOKEN;
}

const int tab[][4] = {
    {'+', 0, PLUS_TOKEN, 0},
    {'-', 0, MINUS_TOKEN, 0},
    {'*', 0, STAR_TOKEN, 0},
    {'/', 0, SLASH_TOKEN, 0},
    {'>', '=', GREATER_TOKEN, GREATER_EQUAL_TOKEN},
    {'<', '=', LESS_TOKEN, LESS_EQUAL_TOKEN},
    {'=', '=', EQUAL_TOKEN, DOUBLE_EQUAL_TOKEN},
    {'&', '&', LOGIC_AND_TOKEN, DOUBLE_LOGIC_AND_TOKEN},
    {'|', '|', LOGIC_OR_TOKEN, DOUBLE_LOGIC_OR_TOKEN},
    {'!', '=', LOGIC_NOT_TOKEN, NOT_EQUAL_TOKEN},
    {'(', 0, LEFT_PARENTHESIS, 0},
    {')', 0, RIGHT_PARENTHESIS, 0},
    {'[', 0, LEFT_BRACKET, 0},
    {']', 0, RIGHT_BRACKET, 0},
    {'{', 0, LEFT_BRACE, 0},
    {'}', 0, RIGHT_BRACE, 0},
    {',', 0, COMMA_TOKEN, 0},
    {';', 0, SEMI_COLON_TOKEN, 0},
    {-1, 0, END_OF_FILE_TOKEN, 0},
};

TokenType __matchTab(Lexer *lexer)
{
    int isDone = 0;
    int index = 0;
    while (!isDone)
    {
        if (tab[index][0] == -1)
            isDone = 1;
        if (tab[index][0] == lexer->__cc)
        {
            // match first
            if (tab[index][1])
            {
                __next(lexer);
                if (tab[index][1] == lexer->__cc)
                {
                    lexer->buf[lexer->len] = lexer->__cc;
                    lexer->len++;
                    lexer->buf[lexer->len] = '\0';
                    lexer->len++;
                    // match second
                    return tab[index][3];
                }
                // match fail, put back
                __putback(lexer);
            }
            lexer->buf[lexer->len] = '\0';
            lexer->len++;
            return tab[index][2];
        }
        index++;
    }
    return 0;
}

void __lex(Lexer *lexer)
{
    lexer->buf[0] = '\0';
    lexer->len = 0;
    __next(lexer);
    if (isDigit(lexer->__cc))
    {
        // lex number
        __lexNumber(lexer);
    }
    else if (isLetter(lexer->__cc) || lexer->__cc == '_')
    {
        // lexer keyword iddentifier
        __lexKeywordOrIdentifier(lexer);
    }
    else if (lexer->__cc == '\"')
    {
        // string
        __lexString(lexer);
    }
    else
    {
        // op
        lexer->buf[lexer->len] = lexer->__cc;
        lexer->len++;
        if (canSkip(lexer->__cc))
        {
            __lex(lexer);
        }
        else if (lexer->__cc == '\n')
        {
            lexer->line++;
            lexer->start = 0;
            __lex(lexer);
        }
        else
        {
            lexer->token = __matchTab(lexer);
            if (!lexer->token)
            {
                reportUnexpectedChar(lexer->line, lexer->start, lexer->__cc);
                __lex(lexer);
            }
        }
    }
}

void initLexer(Lexer *lexer, FILE *in)
{
    // initialize lexer
    lexer->__in = in;
    lexer->buf[0] = '\0';
    lexer->__pc = '\0';
    lexer->__cc = '\0';
    lexer->line = 0;
    lexer->start = 0;
    lexer->len = 0;
}

void next(Lexer *lexer)
{
    if (lexer->token != END_OF_FILE_TOKEN)
        __lex(lexer);
}

int match(Lexer *lexer, TokenType what)
{
    if (lexer->token == what)
    {
        next(lexer);
        return 1;
    }
    else
    {
        reportUnexpectedToken(lexer->line, lexer->start, getTokenTypeValue(lexer->token), getTokenTypeValue(what));
        switch (lexer->token)
        {
        case RIGHT_BRACE:
        case RIGHT_BRACKET:
        case RIGHT_PARENTHESIS:
        case ELSE_TOKEN:
            next(lexer); // drop tokens which can't appear alone
            break;
        default:;
        }
        return 0;
    }
}
