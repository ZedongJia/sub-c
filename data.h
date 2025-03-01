#include "defs.h"

extern char buf[256];
extern unsigned int label;
extern unsigned int has_error;
extern lexer_t *lexer;
extern FILE *cg_out;
extern ast_t *scope;
extern symbol_table_t *var_table; // reference
extern symbol_table_t *func_table;
extern symbol_table_t *const_table;