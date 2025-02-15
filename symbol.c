#include "defs.h"
#include <stdlib.h>
#include <string.h>

int __SymbolTable_try_look_up_var(struct SymbolTable *self, const char *name)
{
    for (int i = 0; i < self->var_size; i++)
    {
        if (strcmp(name, self->vars[i].name) == 0)
            return i;
    }
    return -1;
}

int __SymbolTable_try_declare_var(struct SymbolTable *self, struct CType *ctype, const char *name)
{
    if (__SymbolTable_try_look_up_var(self, name) != -1)
    {
        return 0;
    }
    else
    {
        // insert
        self->vars[self->var_size].ctype = ctype;
        self->vars[self->var_size].name = name;
        self->var_size++;
        return 1;
    }
}

struct SymbolTable *new_symbol_table()
{
    struct SymbolTable *table = (struct SymbolTable *)malloc(sizeof(struct SymbolTable));
    table->var_size = 0;
    table->try_look_up_var = &__SymbolTable_try_look_up_var;
    table->try_declare_var = &__SymbolTable_try_declare_var;
    return table;
}