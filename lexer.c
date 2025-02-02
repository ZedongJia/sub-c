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
    lexer->token = INT_LIT_T;
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
        lexer->token = INT_T;
    else if (strcmp(lexer->buf, "char") == 0)
        lexer->token = CHAR_T;
    else if (strcmp(lexer->buf, "if") == 0)
        lexer->token = IF_T;
    else if (strcmp(lexer->buf, "else") == 0)
        lexer->token = ELSE_T;
    else if (strcmp(lexer->buf, "for") == 0)
        lexer->token = FOR_T;
    else if (strcmp(lexer->buf, "while") == 0)
        lexer->token = WHILE_T;
    else
        lexer->token = ID_T;
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
    lexer->token = STR_LIT_T;
}

const int table[][4] = {
    {'+', 0, PLUS_T, 0},      {'-', 0, MIN_T, 0},        {'*', 0, STAR_T, 0},      {'/', 0, SLASH_T, 0},
    {'>', '=', GT_T, GE_T},   {'<', '=', LT_T, LE_T},    {'=', '=', EQ_T, D_EQ_T}, {'&', '&', AND_T, D_AND_T},
    {'|', '|', OR_T, D_OR_T}, {'!', '=', NOT_T, NE_T},   {'(', 0, L_PAREN_T, 0},   {')', 0, R_PAREN_T, 0},
    {'[', 0, L_BRK_T, 0},     {']', 0, R_BRK_T, 0},      {'{', 0, L_BRC_T, 0},     {'}', 0, R_BRC_T, 0},
    {',', 0, COMMA_T, 0},     {';', 0, SEMI_COLON_T, 0}, {-1, 0, EOF_T, 0},
};

Token __matchTable(Lexer *lexer)
{
    int isDone = 0;
    int index = 0;
    while (!isDone)
    {
        if (table[index][0] == -1)
            isDone = 1;
        if (table[index][0] == lexer->__cc)
        {
            // match first
            if (table[index][1])
            {
                __next(lexer);
                if (table[index][1] == lexer->__cc)
                {
                    lexer->buf[lexer->len] = lexer->__cc;
                    lexer->len++;
                    lexer->buf[lexer->len] = '\0';
                    lexer->len++;
                    // match second
                    return table[index][3];
                }
                // match fail, put back
                __putback(lexer);
            }
            lexer->buf[lexer->len] = '\0';
            lexer->len++;
            return table[index][2];
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
            lexer->token = __matchTable(lexer);
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
    lexer->line = 1;
    lexer->start = 0;
    lexer->len = 0;
}

void next(Lexer *lexer)
{
    if (lexer->token != EOF_T)
        __lex(lexer);
}

int match(Lexer *lexer, Token what)
{
    if (lexer->token == what)
    {
        next(lexer);
        return 1;
    }
    else
    {
        reportUnexpectedToken(lexer->line, lexer->start, tokenName(lexer->token), tokenName(what));
        switch (lexer->token)
        {
        case R_BRC_T:
        case R_BRK_T:
        case R_PAREN_T:
        case ELSE_T:
            next(lexer); // drop tokens which can't appear alone
            break;
        default:;
        }
        return 0;
    }
}
