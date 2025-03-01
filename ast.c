#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ast_append(struct ast *self, struct ast *ast)
{
    self->children->append(self->children, ast, ast->del);
}

list_iterator_t *ast_get_iter(struct ast *self)
{
    return self->children->get_iter(self->children);
}

void ast_del(void *ast)
{
    ast_t *_ast = (ast_t *)ast;
    if (_ast->ctype != NULL)
        free(_ast->ctype);
    if (_ast->value != NULL)
        free(_ast->value);
    if (_ast->children != NULL)
        _ast->children->del(_ast->children);
    if (_ast->var_table != NULL)
        _ast->var_table->del(_ast->var_table);
    free(ast);
}

ast_t *ast_new(syntax_kind_t kind, const char *value)
{
    ast_t *ast = (ast_t *)malloc(sizeof(ast_t));
    // assign ast type
    ast->kind = kind;
    // copy value
    if (value != NULL)
    {
        ast->value = (char *)malloc((strlen(value) + 1) * sizeof(char));
        strcpy(ast->value, value);
    }
    else
    {
        ast->value = NULL;
    }
    // set NULL
    ast->symbol = NULL;
    ast->ctype = NULL;
    ast->children = NULL;
    ast->append = &ast_append;
    ast->get_iter = &ast_get_iter;
    ast->var_table = NULL;
    ast->prt = NULL;
    ast->begin = NULL;
    ast->end = NULL;
    ast->del = &ast_del;
    return ast;
}

ast_t *new_imm(ctype_t *ctype, const char *value)
{
    ast_t *ast = ast_new(IMM_KIND, value);
    ast->ctype = ctype;
    return ast;
}

ast_t *new_literal(ctype_t *ctype, const symbol_t *symbol, const char *value)
{
    ast_t *ast = ast_new(LIT_KIND, value);
    ast->symbol = symbol;
    ast->ctype = ctype;
    return ast;
}

ast_t *new_var(ctype_t *ctype, const symbol_t *symbol, const char *name)
{
    ast_t *ast = ast_new(VAR_KIND, name);
    ast->symbol = symbol;
    ast->ctype = ctype;
    return ast;
}

ast_t *new_unary(syntax_kind_t kind, ctype_t *ctype, ast_t *operand)
{
    ast_t *ast = ast_new(kind, NULL);
    ast->ctype = ctype;
    ast->children = new_list();
    ast->append(ast, operand);
    return ast;
}

ast_t *new_binary(syntax_kind_t kind, ctype_t *ctype, ast_t *left, ast_t *right)
{
    ast_t *ast = ast_new(kind, NULL);
    ast->ctype = ctype;
    ast->children = new_list();
    ast->append(ast, left);
    ast->append(ast, right);
    return ast;
}

ast_t *new_declare(ctype_t *ctype, const char *value, ast_t *initializer)
{
    ast_t *ast = ast_new(DEC_KIND, value);
    ast->ctype = ctype;
    if (initializer != NULL)
    {
        ast->children = new_list();
        ast->append(ast, initializer);
    }
    return ast;
}

ast_t *new_label(int number)
{
    char buf[127];
    sprintf(buf, "label:%d", number);
    ast_t *ast = ast_new(LABEL_KIND, buf);
    return ast;
}

ast_t *new_jump_false(ast_t *condition, const char *value)
{
    ast_t *ast = ast_new(JUMP_FALSE_KIND, value);
    ast->children = new_list();
    ast->append(ast, condition);
    return ast;
}

ast_t *new_jump(const char *value)
{
    ast_t *ast = ast_new(JUMP_KIND, value);
    return ast;
}

ast_t *new_scope(ast_t *prt)
{
    ast_t *ast = ast_new(SCOPE_KIND, NULL);
    ast->prt = prt;
    ast->children = new_list();
    if (prt != NULL)
    {
        ast->var_table = new_symbol_table(prt->var_table);
        ast->begin = prt->begin;
        ast->end = prt->end;
    }
    else
    {
        ast->var_table = new_symbol_table(NULL);
    }
    return ast;
}
