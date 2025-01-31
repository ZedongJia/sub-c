#include "syntax/symbol.h"
#include <string.h>

int tryLookUp(SymbolTable *table, const char *name)
{
    for (int i = 0; i < table->num_var; i++)
    {
        if (strcmp(name, table->variables[i].name) == 0)
            return 1;
    }
    return 0;
}

int tryDeclare(SymbolTable *table, BaseType *baseType, const char *name)
{
    if (tryLookUp(table, name))
    {
        return 0;
    }
    else
    {
        // insert
        table->variables[table->num_var].baseType = baseType;
        table->variables[table->num_var].name = name;
        table->num_var++;
        return 1;
    }
}
