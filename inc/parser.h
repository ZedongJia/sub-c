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
void clearParser(struct Parser *parser);
void parse(struct Parser *parser);
struct Node *parseProgram(struct Parser *parser, int parentPriority);
struct Node *parseCompound(struct Parser *parser, int parentPriority);
struct Node *parseStatement(struct Parser *parser, int parentPriority);
struct Node *parseExpression(struct Parser *parser, int parentPriority);
struct Node *parsePrimaryExpression(struct Parser *parser);
struct Token *step(struct Parser *parser);
struct Token *peek(struct Parser *parser);
#endif