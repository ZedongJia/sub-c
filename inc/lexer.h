#ifndef LEXER
#define LEXER
#include "token.h"
typedef struct Lexer
{
    char buffer[256];
    char peekChar;
    char currentChar;
    Token *peekToken;
    Token *currentToken;
} Lexer;

Lexer *createLexer();
void freeLexer(Lexer *lexer);
void clearLexer(Lexer *lexer);
const char peekChar(Lexer *lexer);
const char nextChar(Lexer *lexer);
Token *lex(Lexer *lexer);
const Token *peekToken(Lexer *lexer);
const Token *nextToken(Lexer *lexer);
int matchToken(Lexer *lexer, TokenType expectedType);
#endif