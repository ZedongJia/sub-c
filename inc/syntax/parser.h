#ifndef PARSER
#define PARSER
#include "syntax/lexer.h"
#include "syntax/node.h"
typedef struct Parser
{
    int labelNumber;
} Parser;
Parser *createParser();
void freeParser(Parser *parser);
Node *parsePrimaryExpression(Parser *parser, Lexer *lexer);
Node *parseUnaryExpression(Parser *parser, Lexer *lexer, int parentPriority);
Node *parseAccessExpression(Node *base, Parser *parser, Lexer *lexer);
Node *parseBinaryExpression(Node *base, Parser *parser, Lexer *lexer, int parentPriority);
Node *parseExpression(Parser *parser, Lexer *lexer, int parentPriority);
Node *parseStatement(Parser *parser, Lexer *lexer);
Node *parseDeclarationStatement(Parser *parser, Lexer *lexer);
Node *parseIfStatement(Parser *parser, Lexer *lexer);
Node *parseElseStatement(Parser *parser, Lexer *lexer);
Node *parseForStatement(Parser *parser, Lexer *lexer);
Node *parseWhileStatement(Parser *parser, Lexer *lexer);
Node *parseStatements(Parser *parser, Lexer *lexer, int isGlobal);
Node *parse(Parser *parser, FILE *file);
#endif