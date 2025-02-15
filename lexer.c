#include "defs.h"
#include "output.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void __Lexer_putbackc(struct Lexer *lexer)
{
    lexer->__pc = lexer->__cc;
}

void __Lexer_getc(struct Lexer *lexer)
{
    lexer->span.col++;
    if (lexer->__pc != '\0')
    {
        lexer->__cc = lexer->__pc;
        lexer->__pc = '\0';
    }
    else
        lexer->__cc = getc(lexer->__in);
}

void __Lexer_putc(struct Lexer *lexer)
{
    lexer->text[lexer->len] = lexer->__cc;
    lexer->len++;
    __Lexer_getc(lexer);
}

void __Lexer_endc(struct Lexer *lexer)
{
    lexer->text[lexer->len] = '\0';
    lexer->len++;
    __Lexer_putbackc(lexer);
}

int __Lexer_can_skip(int ch)
{
    return ch == ' ' || ch == '\r' || ch == '\t';
}

void __Lexer_lex_number(struct Lexer *lexer)
{
    __Lexer_putc(lexer);
    while (isdigit(lexer->__cc))
        __Lexer_putc(lexer);
    __Lexer_endc(lexer);
    lexer->token = INT_LIT_T;
}

void __Lexer_lex_kw_id(struct Lexer *lexer)
{
    __Lexer_putc(lexer);
    while (isalnum(lexer->__cc) || lexer->__cc == '_')
        __Lexer_putc(lexer);
    __Lexer_endc(lexer);
    if (strcmp(lexer->text, "int") == 0)
        lexer->token = INT_T;
    else if (strcmp(lexer->text, "char") == 0)
        lexer->token = CHAR_T;
    else if (strcmp(lexer->text, "if") == 0)
        lexer->token = IF_T;
    else if (strcmp(lexer->text, "else") == 0)
        lexer->token = ELSE_T;
    else if (strcmp(lexer->text, "for") == 0)
        lexer->token = FOR_T;
    else if (strcmp(lexer->text, "while") == 0)
        lexer->token = WHILE_T;
    else if (strcmp(lexer->text, "continue") == 0)
        lexer->token = CONTINUE_T;
    else if (strcmp(lexer->text, "break") == 0)
        lexer->token = BREAK_T;
    else
        lexer->token = ID_T;
}

void __Lexer_lex_string(struct Lexer *lexer)
{
    __Lexer_putc(lexer);
    while (1)
    {
        if (lexer->__cc == '\\')
        {
            __Lexer_putc(lexer);
            __Lexer_putc(lexer);
        }
        else if (lexer->__cc == '\"')
        {
            __Lexer_putc(lexer);
            break;
        }
        else if (lexer->__cc == '\n' || lexer->__cc == -1)
        {
            err_unclosed_str(&lexer->span);
            break;
        }
        else
        {
            __Lexer_putc(lexer);
        }
    }
    __Lexer_endc(lexer);
    lexer->token = STR_LIT_T;
}

const int table[][4] = {
    {'+', 0, PLUS_T, 0},      {'-', 0, MIN_T, 0},        {'*', 0, STAR_T, 0},      {'/', 0, SLASH_T, 0},
    {'>', '=', GT_T, GE_T},   {'<', '=', LT_T, LE_T},    {'=', '=', EQ_T, D_EQ_T}, {'&', '&', AND_T, D_AND_T},
    {'|', '|', OR_T, D_OR_T}, {'!', '=', NOT_T, NE_T},   {'(', 0, L_PAREN_T, 0},   {')', 0, R_PAREN_T, 0},
    {'[', 0, L_BRK_T, 0},     {']', 0, R_BRK_T, 0},      {'{', 0, L_BRC_T, 0},     {'}', 0, R_BRC_T, 0},
    {',', 0, COMMA_T, 0},     {';', 0, SEMI_COLON_T, 0}, {-1, 0, EOF_T, 0},
};

Token __Lexer_match_table(struct Lexer *lexer)
{
    Token token = 0;
    int index = 0;
    while (table[index][0] != -1)
    {
        // match first
        if (table[index][0] == lexer->__cc)
        {
            __Lexer_putc(lexer);
            token = table[index][2];
            // match second
            if (table[index][1] && table[index][1] == lexer->__cc)
            {
                __Lexer_putc(lexer);
                token = table[index][3];
            }
            __Lexer_endc(lexer);
            break;
        }
        index++;
    }
    return token;
}

void __Lexer_lex(struct Lexer *lexer)
{
    lexer->text[0] = '\0';
    lexer->len = 0;
    __Lexer_getc(lexer);
    if (isdigit(lexer->__cc))
    {
        // lex number
        __Lexer_lex_number(lexer);
    }
    else if (isalpha(lexer->__cc) || lexer->__cc == '_')
    {
        // lexer keyword iddentifier
        __Lexer_lex_kw_id(lexer);
    }
    else if (lexer->__cc == '\"')
    {
        // string
        __Lexer_lex_string(lexer);
    }
    else if (__Lexer_can_skip(lexer->__cc))
    {
        __Lexer_lex(lexer);
    }
    else if (lexer->__cc == '\n')
    {
        lexer->span.row++;
        lexer->span.col = 0;
        __Lexer_lex(lexer);
    }
    else if (lexer->__cc == -1)
    {
        lexer->token = EOF_T;
    }
    else
    {
        lexer->token = __Lexer_match_table(lexer);
        if (!lexer->token)
        {
            err_unexpect_char(&lexer->span, lexer->__cc);
            __Lexer_lex(lexer);
        }
    }
}

void __Lexer_next(struct Lexer *lexer)
{
    __Lexer_lex(lexer);
}

/// @brief match current token with `what`, go next and return 1 if success else conditionally go next and return 0
int __Lexer_match(struct Lexer *lexer, Token what)
{
    if (lexer->token == what)
    {
        if (lexer->token != EOF_T)
            __Lexer_next(lexer);
        return 1;
    }
    else
    {
        err_unexpect_token(&lexer->span, lexer->token);
        switch (lexer->token)
        {
        case R_BRC_T:
        case R_BRK_T:
        case R_PAREN_T:
        case ELSE_T:
            __Lexer_next(lexer); // drop tokens which can't appear alone
            break;
        default:;
        }
        return 0;
    }
}

void __Lexer_del(struct Lexer *lexer)
{
    free(lexer);
}

struct Lexer *new_lexer(FILE *in)
{
    struct Lexer *lexer = (struct Lexer *)malloc(sizeof(struct Lexer));
    // initialize lexer
    lexer->__in = in;
    lexer->__pc = '\0';
    lexer->__cc = '\0';

    // initialize span
    lexer->text[0] = '\0';
    lexer->len = 0;
    lexer->span.row = 1;
    lexer->span.col = 0;

    // bind function
    lexer->next = &__Lexer_next;
    lexer->match = &__Lexer_match;
    lexer->del = &__Lexer_del;
    return lexer;
}