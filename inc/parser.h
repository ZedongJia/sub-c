#ifndef PARSER
#define PARSER
#include "lexer.h"
#include "node.h"
struct Parser
{
    struct Lexer *lexer;
    struct Node *root;
    struct Token *peekToken;
    struct Token *currentToken;
};
struct Parser *createParser();
void freeParser(struct Parser *parser);
void parse(struct Parser *parser);
struct Node *parseExpression(struct Parser *parser, int parentPriority);
struct Node *parsePrimaryExpression(struct Parser *parser);
struct Token *step(struct Parser *parser);
struct Token *peek(struct Parser *parser);
#endif