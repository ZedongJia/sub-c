#include "utils.h"

void __Err_position(struct Span *span)
{
    printf("Error at row: %d, col: %d: ", span->row, span->col);
}

void __Err_end()
{
    printf("\n");
}

void __err_unclosed_str(struct Span *span)
{
    __Err_position(span);
    printf("unclosed string");
    __Err_end();
}

void __err_unexpect_char(struct Span *span, const char c)
{
    __Err_position(span);
    printf("unexpect %c", c);
    __Err_end();
}

void __err_unexpect_token(struct Span *span, const Token token)
{
    __Err_position(span);
    printf("unexpect %s", token_name(token));
    __Err_end();
}

void __err_var_undefined(struct Span *span, const char *var)
{
    __Err_position(span);
    printf("variable %s undefined", var);
    __Err_end();
}

void __err_var_defined(struct Span *span, const char *var)
{
    __Err_position(span);
    printf("variable %s already defined", var);
    __Err_end();
}

void __err_incompat_binary(struct Span *span, struct CType *left, Kind op, struct CType *right)
{
    __Err_position(span);
    printf("not support");
    __debug_print_CType(left);
    printf(" %s ", kind_name(op));
    __debug_print_CType(right);
    __Err_end();
}

void __err_incompat_unary(struct Span *span, Kind op, struct CType *operand)
{
    __Err_position(span);
    printf("not support %s", kind_name(op));
    __debug_print_CType(operand);
    __Err_end();
}
