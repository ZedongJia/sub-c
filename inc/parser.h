#ifndef PARSER
#define PARSER
#include "lexer.h"
#include "node.h"
typedef struct Parser
{
    Lexer *lexer;
    Node *root;
} Parser;
Parser *createParser();
void freeParser(Parser *parser);
Node *parseUnaryExpression(Parser *parser, int parentPriority);
Node *parsePrimaryExpression(Parser *parser);
Node *parseExpression(Parser *parser, int parentPriority);
Node *parseStatement(Parser *parser, int parentPriority);
Node *parseDeclarationStatement(Parser *parser, int parentPriority);
Node *parseCompound(Parser *parser, int parentPriority);
Node *parseProgram(Parser *parser, int parentPriority);
void parse(Parser *parser);
#endif