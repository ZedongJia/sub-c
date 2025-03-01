#include "defs.h"
#include <stdio.h>
#include <stdlib.h>

char buf[256];
unsigned int label;
unsigned int has_error;
lexer_t *lexer;
FILE *cg_out;
ast_t *scope;
// reference from scope ast
symbol_table_t *var_table;
symbol_table_t *func_table;
symbol_table_t *const_table;

void init(FILE *in, FILE *out)
{
    label = 0;
    has_error = 0;
    lexer = new_lexer(in);
    cg_out = out;
    scope = new_scope(NULL);
    var_table = scope->var_table;
    func_table = new_symbol_table(NULL);
    const_table = new_symbol_table(NULL);
}

void del()
{
    lexer->del(lexer);
    scope->del(scope);
    func_table->del(func_table);
    const_table->del(const_table);
}

int main(int argv, const char *argc[])
{
    init(stdin, stdout);
    while (1)
    {
        printf("[IN]\n");
        parse_program();
        debug_pretty_tree(scope);
        if (!has_error)
        {
            printf("[ASM]\n");
            gen_program();
        }
    }
    del();
}