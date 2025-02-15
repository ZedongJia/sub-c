#include "output.h"

void __err_position(struct Span *span)
{
    printf("Error at row: %d, col: %d: ", span->row, span->col);
}

void __err_end()
{
    printf("\n");
}

void err_unclosed_str(struct Span *span)
{
    __err_position(span);
    printf("unclosed string");
    __err_end();
}

void err_unexpect_char(struct Span *span, const char c)
{
    __err_position(span);
    printf("unexpect %c", c);
    __err_end();
}

void err_unexpect_token(struct Span *span, const Token token)
{
    __err_position(span);
    printf("unexpect %s", token_name(token));
    __err_end();
}

void err_var_undefined(struct Span *span, const char *var)
{
    __err_position(span);
    printf("variable %s undefined", var);
    __err_end();
}

void err_var_defined(struct Span *span, const char *var)
{
    __err_position(span);
    printf("variable %s already defined", var);
    __err_end();
}

void err_incompat_binary(struct Span *span, struct CType *left, Kind op, struct CType *right)
{
    __err_position(span);
    printf("not support ");
    debug_print_CType(left);
    printf(" %s ", kind_name(op));
    debug_print_CType(right);
    __err_end();
}

void err_incompat_unary(struct Span *span, Kind op, struct CType *operand)
{
    __err_position(span);
    printf("not support %s", kind_name(op));
    debug_print_CType(operand);
    __err_end();
}

void err_continus(struct Span *span)
{
    __err_position(span);
    printf("continue must be used in a loop");
    __err_end();
}

void err_break(struct Span *span)
{
    __err_position(span);
    printf("break must be used in a loop");
    __err_end();
}