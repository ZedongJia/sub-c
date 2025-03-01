#include "defs.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void lexer_putbackc(lexer_t *lexer)
{
    lexer->pc = lexer->cc;
}

void lexer_getc(lexer_t *lexer)
{
    lexer->span.col++;
    if (lexer->pc != '\0')
    {
        lexer->cc = lexer->pc;
        lexer->pc = '\0';
    }
    else
        lexer->cc = getc(lexer->in);
}

void lexer_putc(lexer_t *lexer)
{
    lexer->text[lexer->len] = lexer->cc;
    lexer->len++;
    lexer_getc(lexer);
}

void lexer_endc(lexer_t *lexer)
{
    lexer->text[lexer->len] = '\0';
    lexer->len++;
    lexer_putbackc(lexer);
}

int lexer_can_skip(int ch)
{
    return ch == ' ' || ch == '\r' || ch == '\t';
}

void lexer_parse_number(lexer_t *lexer)
{
    lexer_putc(lexer);
    while (isdigit(lexer->cc))
        lexer_putc(lexer);
    lexer_endc(lexer);
    lexer->token = INT_LIT_T;
}

void lexer_parse_kw_id(lexer_t *lexer)
{
    lexer_putc(lexer);
    while (isalnum(lexer->cc) || lexer->cc == '_')
        lexer_putc(lexer);
    lexer_endc(lexer);
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

void lexer_parse_string(lexer_t *lexer)
{
    lexer_getc(lexer);
    while (1)
    {
        if (lexer->cc == '\\')
        {
            lexer_putc(lexer);
            lexer_putc(lexer);
        }
        else if (lexer->cc == '\"')
        {
            lexer_getc(lexer);
            break;
        }
        else if (lexer->cc == '\n' || lexer->cc == -1)
        {
            err_unclosed_str(&lexer->span);
            break;
        }
        else
        {
            lexer_putc(lexer);
        }
    }
    lexer_endc(lexer);
    lexer->token = STR_LIT_T;
}

const int op_table[][4] = {
    {'+', 0, PLUS_T, 0},      {'-', 0, MIN_T, 0},        {'*', 0, STAR_T, 0},      {'/', 0, SLASH_T, 0},
    {'>', '=', GT_T, GE_T},   {'<', '=', LT_T, LE_T},    {'=', '=', EQ_T, D_EQ_T}, {'&', '&', AND_T, D_AND_T},
    {'|', '|', OR_T, D_OR_T}, {'!', '=', NOT_T, NE_T},   {'(', 0, L_PAREN_T, 0},   {')', 0, R_PAREN_T, 0},
    {'[', 0, L_BRK_T, 0},     {']', 0, R_BRK_T, 0},      {'{', 0, L_BRC_T, 0},     {'}', 0, R_BRC_T, 0},
    {',', 0, COMMA_T, 0},     {';', 0, SEMI_COLON_T, 0}, {-1, 0, EOF_T, 0},
};

token_t lexer_match_operator(lexer_t *lexer)
{
    token_t token = 0;
    int index = 0;
    while (op_table[index][0] != -1)
    {
        // match first
        if (op_table[index][0] == lexer->cc)
        {
            lexer_putc(lexer);
            token = op_table[index][2];
            // match second
            if (op_table[index][1] && op_table[index][1] == lexer->cc)
            {
                lexer_putc(lexer);
                token = op_table[index][3];
            }
            lexer_endc(lexer);
            break;
        }
        index++;
    }
    return token;
}

void lexer_lex(lexer_t *lexer)
{
    lexer->text[0] = '\0';
    lexer->len = 0;
    lexer_getc(lexer);
    if (isdigit(lexer->cc))
    {
        // lex number
        lexer_parse_number(lexer);
    }
    else if (isalpha(lexer->cc) || lexer->cc == '_')
    {
        // lexer keyword iddentifier
        lexer_parse_kw_id(lexer);
    }
    else if (lexer->cc == '\"')
    {
        // string
        lexer_parse_string(lexer);
    }
    else if (lexer_can_skip(lexer->cc))
    {
        lexer_lex(lexer);
    }
    else if (lexer->cc == '\n')
    {
        lexer->span.row++;
        lexer->span.col = 0;
        lexer_lex(lexer);
    }
    else if (lexer->cc == -1)
    {
        lexer->token = EOF_T;
    }
    else
    {
        lexer->token = lexer_match_operator(lexer);
        if (!lexer->token)
        {
            err_unexpect_char(&lexer->span, lexer->cc);
            lexer_lex(lexer);
        }
    }
}

void lexer_next(lexer_t *lexer)
{
    lexer_lex(lexer);
}

/// @brief match current token with `what`, go next and return 1 if success else conditionally go next and return 0
int lexer_match(lexer_t *lexer, token_t what)
{
    if (lexer->token == what)
    {
        if (lexer->token != EOF_T)
            lexer_next(lexer);
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
            lexer_next(lexer); // drop tokens which can't appear alone
            break;
        default:;
        }
        return 0;
    }
}

void lexer_del(lexer_t *lexer)
{
    free(lexer);
}

lexer_t *new_lexer(FILE *in)
{
    lexer_t *lexer = (lexer_t *)malloc(sizeof(lexer_t));
    // initialize lexer
    lexer->in = in;
    lexer->pc = '\0';
    lexer->cc = '\0';

    // initialize span
    lexer->text[0] = '\0';
    lexer->len = 0;
    lexer->span.row = 1;
    lexer->span.col = 0;

    // bind function
    lexer->next = &lexer_next;
    lexer->match = &lexer_match;
    lexer->del = &lexer_del;
    return lexer;
}