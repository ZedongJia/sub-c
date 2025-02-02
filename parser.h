#include "ast.h"
#ifndef PARSER
#define PARSER
typedef struct Parser
{
    int number;    // label number
    ASTNode *curr; // current scope
} Parser;
Parser *createParser();
void freeParser(Parser *parser);
ASTNode *enterScope(Parser *parser);
void leaveScope(Parser *parser);
#endif