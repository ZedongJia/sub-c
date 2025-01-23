#ifndef PARSER
#define PARSER
#include "lexer.h"
#include "node.h"
struct Parser
{
    struct Lexer *lexer;
    struct Node *root;
};
struct Parser *createParser();
void freeParser(struct Parser *parser);
struct Node *parseUnaryExpression(struct Parser *parser, int parentPriority);
struct Node *parsePrimaryExpression(struct Parser *parser);
struct Node *parseExpression(struct Parser *parser, int parentPriority);
struct Node *parseStatement(struct Parser *parser, int parentPriority);
struct Node *parseDeclarationStatement(struct Parser *parser, int parentPriority);
struct Node *parseCompound(struct Parser *parser, int parentPriority);
struct Node *parseProgram(struct Parser *parser, int parentPriority);
void parse(struct Parser *parser);
#endif