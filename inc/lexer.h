#ifndef LEXER
#define LEXER
#include "token.h"
struct Lexer
{
    char buffer[256];
    char peekChar;
    char currentChar;
    struct Token *peekToken;
    struct Token *currentToken;
};

struct Lexer *createLexer();
void freeLexer(struct Lexer *lexer);
void clearLexer(struct Lexer *lexer);
const char peekChar(struct Lexer *lexer);
const char nextChar(struct Lexer *lexer);
struct Token *lex(struct Lexer *lexer);
const struct Token *peekToken(struct Lexer *lexer);
const struct Token *nextToken(struct Lexer *lexer);
#endif