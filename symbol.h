#ifndef SYMBOL
#define SYMBOL
#include "types.h"

typedef struct VariableSymbol
{
    CType *ctype;
    const char *name;
} VariableSymbol;

typedef struct SymbolTable
{
    VariableSymbol variables[256];
    int num_var;
} SymbolTable;

int tryLookUp(SymbolTable *table, const char *name);
int tryDeclare(SymbolTable *table, CType *ctype, const char *name);
#endif