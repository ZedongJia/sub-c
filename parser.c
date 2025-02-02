#include "parser.h"
#include <stdlib.h>

Parser *createParser()
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->number = 0;
    parser->curr = NULL;
    return parser;
}

void freeParser(Parser *parser)
{
    free(parser);
}

ASTNode *enterScope(Parser *parser)
{
    // enter new scope
    ASTNode *scope = cScope(parser->curr);
    parser->curr = scope;
    return scope;
}

void leaveScope(Parser *parser)
{
    // back to parent scope
    ASTNode *scope = parser->curr;
    parser->curr = scope->parent;
    if (parser->curr != NULL)
        appendToList(parser->curr->children, scope);
}