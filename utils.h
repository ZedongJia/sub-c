#ifndef UTILS
#define UTILS
#include "defs.h"
#include "enums.h"

/**
 *     Debug
 */
void __debug_print_CType(const struct CType *ctype);
void __debug_pretty_tree(struct ASTNode *node);

/**
 *     Error
 */
void __err_unclosed_str(struct Span *span);
void __err_unexpect_char(struct Span *span, const char c);
void __err_unexpect_token(struct Span *span, const Token token);
void __err_var_undefined(struct Span *span, const char *var);
void __err_var_defined(struct Span *span, const char *var);
void __err_incompat_binary(struct Span *span, struct CType *left, Kind op, struct CType *right);
void __err_incompat_unary(struct Span *span, Kind op, struct CType *operand);

#endif