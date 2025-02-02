#include "report.h"
#include "token.h"
#include <stdio.h>
#ifndef LEXER
#define LEXER
typedef struct Lexer
{
    FILE *__in;      // lex file
    char buf[255]; // buf
    char __pc;       // putback char
    char __cc;       // current char
    TokenType token;
    int line;  // file line
    int start; // buf start
    int len;   // bug end
} Lexer;

void initLexer(Lexer *lexer, FILE *in);
void next(Lexer *lexer);
/// @brief match current token with `what`, go next and return 1 if success else conditionally go next and return 0
int match(Lexer *lexer, TokenType expectedTokenType);
#endif