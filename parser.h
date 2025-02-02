#ifndef PARSER
#define PARSER
#include "ast.h"
#include "lexer.h"
typedef struct Parser
{
    int number;    // label number
    ASTNode *curr; // current scope
    void (*append)(struct Parser *parser, void *data);
    void (*enter)(struct Parser *parser);
    void (*leave)(struct Parser *parser);
    void (*del)(struct Parser *parser);
} Parser;

Parser *createParser();

// expression

ASTNode *parsePrimary(Parser *parser, Lexer *lexer);
ASTNode *parsePrefix(Parser *parser, Lexer *lexer, int parentPriority);
ASTNode *parseSuffix(ASTNode *left, Parser *parser, Lexer *lexer);
ASTNode *parseBinary(ASTNode *left, Parser *parser, Lexer *lexer, int parentPriority);
ASTNode *parseExpression(Parser *parser, Lexer *lexer, int parentPriority);

// statement
void parseStatement(Parser *parser, Lexer *lexer);
void parseDeclare(Parser *parser, Lexer *lexer);
void parseIf(Parser *parser, Lexer *lexer);
void parseElse(Parser *parser, Lexer *lexer, ASTNode *trueEnd);
void parseFor(Parser *parser, Lexer *lexer);
void parseWhile(Parser *parser, Lexer *lexer);
void parseStatements(Parser *parser, Lexer *lexer);
void parseProgram(Parser *parser, FILE *in);
#endif