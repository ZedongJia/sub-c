#include "data.h"
#include "defs.h"
#include "stdlib.h"

void debug_pretty_prefix(int *parr, int indent, int isLast, const char *end)
{
    printf("\n");
    for (int i = 0; i < indent; i++)
        printf(parr[i] ? "    " : "│   ");
    printf(isLast ? "└─%s " : "├─%s ", end);
}

void debug_pretty_kind(syntax_kind_t kind)
{
    printf("\033[33;1m%s\033[0m", syntax_kind_name(kind));
}

void debug_pretty_name(const char *name)
{
    printf("\033[36;1m%s\033[0m", name);
}

void debug_print_ctype(const ctype_t *ctype)
{
    if (!ctype->mut)
        printf("const ");
    else
        printf("mutable ");
    printf("%s", type_name(ctype->type));
    if (ctype->ptr)
    {
        for (int i = 0; i < ctype->ptr; i++)
            printf("*");
    }
}

void __debug_pretty_tree(ast_t *ast, int *parr, int indent, int isLast)
{
    debug_pretty_prefix(parr, indent, isLast, "─");
    parr[indent] = isLast ? 1 : 0;
    debug_pretty_kind(ast->kind);
    // type
    if (ast->ctype != NULL)
    {
        debug_pretty_prefix(parr, indent + 1, ast->value == NULL && ast->children == NULL, "*");
        debug_pretty_name("Type: ");
        debug_print_ctype(ast->ctype);
        printf("(%dbytes)", ast->ctype->offset[ast->ctype->ptr]);
    }
    // value
    if (ast->value != NULL)
    {
        debug_pretty_prefix(parr, indent + 1, ast->children == NULL, "*");
        debug_pretty_name("Value: ");
        printf("%s", ast->value);
    }
    // children
    if (ast->children != NULL)
    {
        list_iterator_t *iter = ast->children->get_iter(ast->children);
        while (!iter->is_end(iter))
        {
            void *data = iter->get(iter);
            iter->next(iter);
            __debug_pretty_tree((ast_t *)data, parr, indent + 1, iter->is_end(iter));
        }
        iter->del(iter);
    }
}

void debug_pretty_tree(ast_t *ast)
{
    if (ast == NULL)
        return;
    int parr[256];
    for (int i = 0; i < 256; i++)
        parr[i] = 0;

    printf("[Tree]");
    __debug_pretty_tree(ast, parr, 0, 1);
    printf("\n");
}

void err_position(span_t *span)
{
    has_error = 1;
    printf("Error at row: %d, col: %d: ", span->row, span->col);
}

void err_end()
{
    printf("\n");
}

void err_unclosed_str(span_t *span)
{
    err_position(span);
    printf("unclosed string");
    err_end();
}

void err_unexpect_char(span_t *span, const char c)
{
    err_position(span);
    printf("unexpect %c", c);
    err_end();
}

void err_unexpect_token(span_t *span, const token_t token)
{
    err_position(span);
    printf("unexpect %s", token_name(token));
    err_end();
}

void err_var_undefined(span_t *span, const char *var)
{
    err_position(span);
    printf("variable %s undefined", var);
    err_end();
}

void err_var_defined(span_t *span, const char *var)
{
    err_position(span);
    printf("variable %s already defined", var);
    err_end();
}

void err_incompat_binary(span_t *span, ctype_t *left, syntax_kind_t op, ctype_t *right)
{
    err_position(span);
    printf("not support ");
    debug_print_ctype(left);
    printf(" %s ", syntax_kind_name(op));
    debug_print_ctype(right);
    err_end();
}

void err_incompat_unary(span_t *span, syntax_kind_t op, ctype_t *operand)
{
    err_position(span);
    printf("not support %s", syntax_kind_name(op));
    debug_print_ctype(operand);
    err_end();
}

void err_continus(span_t *span)
{
    err_position(span);
    printf("continue must be used in a loop");
    err_end();
}

void err_break(span_t *span)
{
    err_position(span);
    printf("break must be used in a loop");
    err_end();
}