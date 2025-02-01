#include "output/report.h"
#include "syntax/token.h"
#include <stdio.h>
#ifndef LEXER
#define LEXER
typedef struct Lexer
{
    // private

    FILE *__file;
    char __buffer[255];
    char __postChar;
    char __currChar;

    // public

    Token *postToken;
    Token *currToken;

    // position
    int line;
    int column;
} Lexer;

Lexer *createLexer(FILE *file);
void freeLexer(Lexer *lexer);

void __peekChar(Lexer *lexer);
void __nextChar(Lexer *lexer);
Token *__lexNumber(Lexer *lexer);
Token *__lexKeywordOrIdentifier(Lexer *lexer);
Token *__lexString(Lexer *lexer);
Token *__lex(Lexer *lexer);

void peekToken(Lexer *lexer);
void nextToken(Lexer *lexer);
int matchToken(Lexer *lexer, TokenType expectedTokenType);
#endif