#ifndef EXP
#define EXP
#include "lexer.h"
#include "parser.h"

ASTNode *parsePrimary(Parser *parser, Lexer *lexer);
ASTNode *parsePrefix(Parser *parser, Lexer *lexer, int parentPriority);
ASTNode *parseSuffix(ASTNode *left, Parser *parser, Lexer *lexer);
ASTNode *parseBinary(ASTNode *left, Parser *parser, Lexer *lexer, int parentPriority);
ASTNode *parseExpression(Parser *parser, Lexer *lexer, int parentPriority);
#endif