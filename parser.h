#include "ast.h"
#include "lexer.h"
#ifndef PARSER
#define PARSER
typedef struct Parser
{
    int number;    // label number
    ASTNode *curr; // current scope
} Parser;
Parser *createParser();
void freeParser(Parser *parser);
ASTNode *parsePrimaryExpression(Parser *parser, Lexer *lexer);
ASTNode *parseUnaryExpression(Parser *parser, Lexer *lexer, int parentPriority);
ASTNode *parseAccessExpression(ASTNode *base, Parser *parser, Lexer *lexer);
ASTNode *parseBinaryExpression(ASTNode *base, Parser *parser, Lexer *lexer, int parentPriority);
ASTNode *parseExpression(Parser *parser, Lexer *lexer, int parentPriority);
void parseStatement(Parser *parser, Lexer *lexer);
BaseType *parsePointerType(BaseType *baseType, Parser *parser, Lexer *lexer);
BaseType *parseArrayType(BaseType *baseType, Parser *parser, Lexer *lexer);
void parseDeclarationStatement(Parser *parser, Lexer *lexer);
void parseIfStatement(Parser *parser, Lexer *lexer);
void parseElseStatement(Parser *parser, Lexer *lexer);
void parseForStatement(Parser *parser, Lexer *lexer);
void parseWhileStatement(Parser *parser, Lexer *lexer);
ASTNode *parseStatements(Parser *parser, Lexer *lexer, int isGlobal);
ASTNode *parse(Parser *parser, FILE *in);
ASTNode *enterScope(Parser *parser);
void leaveScope(Parser *parser);
#endif