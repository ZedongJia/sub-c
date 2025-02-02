#include "types.h"
#ifndef SYMBOL
#define SYMBOL

typedef struct VariableSymbol
{
    BaseType *baseType;
    const char *name;
} VariableSymbol;

typedef struct SymbolTable
{
    VariableSymbol variables[256];
    int num_var;
} SymbolTable;

int tryLookUp(SymbolTable *table, const char *name);
int tryDeclare(SymbolTable *table, BaseType *baseType, const char *name);
#endif