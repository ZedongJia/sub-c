#ifndef LEXER
#define LEXER
#include "token.h"
struct Lexer
{
    char text[256];
    char peekChar;
    char currentChar;
};

struct Lexer *createLexer();
void freeLexer(struct Lexer *lexer);
struct Token *nextToken(struct Lexer *lexer);
char nextChar(struct Lexer *lexer);
char peekChar(struct Lexer *lexer);
#endif