#include "parser.h"
#include <stdlib.h>

void __Parser_append(Parser *self, void *data)
{
    List *list = self->curr->children;
    list->append(list, data);
}

void __Parser_del(Parser *self)
{
    ASTNode_del(self->curr);
    free(self);
}

void __Parser_enter(Parser *self)
{
    // enter new scope
    self->curr = ASTNode_cScope(self->curr);
}

void __Parser_leave(Parser *self)
{
    // back to parent scope
    if (self->curr->parent == NULL)
        return;
    ASTNode *scope = self->curr;
    self->curr = scope->parent;
    self->append(self, scope);
}

Parser *createParser()
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->number = 0;
    parser->curr = ASTNode_cScope(NULL);
    parser->append = &__Parser_append;
    parser->enter = &__Parser_enter;
    parser->leave = &__Parser_leave;
    parser->del = &__Parser_del;
    return parser;
}
