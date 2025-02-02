#ifndef STMT
#define STMT
#include "lexer.h"
#include "parser.h"

void parseStatement(Parser *parser, Lexer *lexer);
void parseDeclare(Parser *parser, Lexer *lexer);
void parseIf(Parser *parser, Lexer *lexer);
void parseElse(Parser *parser, Lexer *lexer, ASTNode *trueEnd);
void parseFor(Parser *parser, Lexer *lexer);
void parseWhile(Parser *parser, Lexer *lexer);
void parseStatements(Parser *parser, Lexer *lexer);
ASTNode *parseProgram(Parser *parser, FILE *in);
#endif