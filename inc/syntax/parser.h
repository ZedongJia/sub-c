#ifndef PARSER
#define PARSER
#include "syntax/lexer.h"
#include "syntax/node.h"
typedef struct Parser
{
    int labelNumber;
    Scope *currScope;
} Parser;
Parser *createParser();
void freeParser(Parser *parser);
Node *parsePrimaryExpression(Parser *parser, Lexer *lexer);
Node *parseUnaryExpression(Parser *parser, Lexer *lexer, int parentPriority);
Node *parseAccessExpression(Node *base, Parser *parser, Lexer *lexer);
Node *parseBinaryExpression(Node *base, Parser *parser, Lexer *lexer, int parentPriority);
Node *parseExpression(Parser *parser, Lexer *lexer, int parentPriority);
void parseStatement(Parser *parser, Lexer *lexer);
void parseDeclarationStatement(Parser *parser, Lexer *lexer);
void parseIfStatement(Parser *parser, Lexer *lexer);
void parseElseStatement(Parser *parser, Lexer *lexer);
void parseForStatement(Parser *parser, Lexer *lexer);
void parseWhileStatement(Parser *parser, Lexer *lexer);
Node *parseStatements(Parser *parser, Lexer *lexer, int isGlobal);
Node *parse(Parser *parser, FILE *file);
#endif