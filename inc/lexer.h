#ifndef LEXER
#define LEXER
#include "token.h"
struct Lexer
{
    char text[256];
    char peekChar;
    char currentChar;
    struct Token *peekToken;
    struct Token *currentToken;
};

struct Lexer *createLexer();
void freeLexer(struct Lexer *lexer);
void clearLexer(struct Lexer *lexer);
struct Token *lex(struct Lexer *lexer);
char peekChar(struct Lexer *lexer);
char nextChar(struct Lexer *lexer);
struct Token *peekToken(struct Lexer *lexer);
struct Token *nextToken(struct Lexer *lexer);
#endif