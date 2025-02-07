#include "defs.h"
#include <stdlib.h>

void __Parser_append(struct Parser *self, struct ASTNode *data)
{
    struct List *list = self->curr->children;
    list->append(list, data, data->del);
}

void __Parser_del(struct Parser *self)
{
    self->curr->del(self->curr);
    self->lexer->del(self->lexer);
    free(self);
}

void __Parser_enter(struct Parser *self)
{
    // enter new scope
    self->curr = new_scope(self->curr);
}

void __Parser_leave(struct Parser *self)
{
    // back to prt scope
    if (self->curr->prt == NULL)
        return;
    struct ASTNode *scope = self->curr;
    self->curr = scope->prt;
    self->append(self, scope);
}

void __Parser_next(struct Parser *self)
{
    self->lexer->next(self->lexer);
}

const char *__Parser_value(struct Parser *self)
{
    return self->lexer->text;
}

Token __Parser_token(struct Parser *self)
{
    return self->lexer->token;
}

struct Span __Parser_span(struct Parser *self)
{
    return self->lexer->span;
}

int __Parser_match(struct Parser *self, Token what)
{
    return self->lexer->match(self->lexer, what);
}

struct Parser *new_parser(FILE *in)
{
    struct Parser *parser = (struct Parser *)malloc(sizeof(struct Parser));
    parser->number = 0;

    // bind lexer
    parser->lexer = new_lexer(in);
    parser->next = &__Parser_next;
    parser->token = &__Parser_token;
    parser->span = &__Parser_span;
    parser->value = &__Parser_value;
    parser->match = &__Parser_match;

    // bind list
    parser->curr = new_scope(NULL);
    parser->append = &__Parser_append;
    parser->enter = &__Parser_enter;
    parser->leave = &__Parser_leave;
    parser->del = &__Parser_del;
    return parser;
}
