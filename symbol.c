#include "defs.h"
#include <string.h>

int try_look_up(struct SymbolTable *table, const char *name)
{
    for (int i = 0; i < table->num_var; i++)
    {
        if (strcmp(name, table->variables[i].name) == 0)
            return i;
    }
    return -1;
}

int try_declare(struct SymbolTable *table, struct CType *ctype, const char *name)
{
    if (try_look_up(table, name) != -1)
    {
        return 0;
    }
    else
    {
        // insert
        table->variables[table->num_var].ctype = ctype;
        table->variables[table->num_var].name = name;
        table->num_var++;
        return 1;
    }
}
