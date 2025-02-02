#include "types.h"
#include <stdlib.h>

char *typeName(Type valueType)
{
    switch (valueType)
    {
    case INT_VALUE:
        return "int";
    case CHAR_VALUE:
        return "char";
    case BOOL_VALUE:
        return "bool";
    default:
        return "?";
    }
}

Type toType(Token tokenType)
{
    switch (tokenType)
    {
    case INT_T:
        return INT_VALUE;
    case CHAR_T:
        return CHAR_VALUE;
    default:
        return 0;
    }
}

CType *createCType(Type type)
{
    CType *ctype = (CType *)malloc(sizeof(CType));
    ctype->type = type;
    switch (type)
    {
    case INT_VALUE:
        ctype->offset[0] = 4;
        break;
    case CHAR_VALUE:
        ctype->offset[0] = 1;
        break;
    case BOOL_VALUE:
        ctype->offset[0] = 1;
        break;
    default:
        ctype->offset[0] = 0;
        break;
    }
    ctype->ptr = 0;
    ctype->dim = 0;
    return ctype;
}
void freeCType(CType *ctype)
{
    free(ctype);
}

void point(CType *ctype)
{
    ctype->ptr = ctype->ptr + 1;
}

void array(CType *ctype, int size)
{
    // TODO: dim check
    ctype->offset[ctype->dim + 1] = ctype->offset[ctype->dim] * size;
    ctype->dim++;
}
