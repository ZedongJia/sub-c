#ifndef PARSER
#define PARSER
#include "syntax/lexer.h"
#include "syntax/node.h"
typedef struct Parser
{
} Parser;
Parser *createParser();
void freeParser(Parser *parser);
Node *parsePrimaryExpression(Parser *parser, Lexer *lexer);
Node *parseUnaryExpression(Parser *parser, Lexer *lexer, int parentPriority);
Node *parseExpression(Parser *parser, Lexer *lexer, int parentPriority);
Node *parseStatement(Parser *parser, Lexer *lexer, int parentPriority);
Node *parseDeclarationStatement(Parser *parser, Lexer *lexer, int parentPriority);
Node *parseStatements(Parser *parser, Lexer *lexer, int parentPriority, int isGlobal);
Node *parse(Parser *parser, FILE *file);
#endif