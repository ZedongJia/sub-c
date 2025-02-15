#ifndef OUTPUT
#define OUTPUT
#include "defs.h"
#include "enums.h"

/**
 *     Debug
 */
void debug_print_CType(const struct CType *ctype);
void debug_pretty_tree(struct ASTNode *node);

/**
 *     Error
 */
void err_unclosed_str(struct Span *span);
void err_unexpect_char(struct Span *span, const char c);
void err_unexpect_token(struct Span *span, const Token token);
void err_var_undefined(struct Span *span, const char *var);
void err_var_defined(struct Span *span, const char *var);
void err_incompat_binary(struct Span *span, struct CType *left, Kind op, struct CType *right);
void err_incompat_unary(struct Span *span, Kind op, struct CType *operand);
void err_continus(struct Span *span);
void err_break(struct Span *span);

#endif