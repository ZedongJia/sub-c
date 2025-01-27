#include "syntax/types.h"

char *getBaseTypeValue(BaseType kind)
{
    switch (kind)
    {
    case UNEXPECTED_TYPE:
        return "?";
    case INT_TYPE:
        return "int";
    case CHAR_TYPE:
        return "char";
    default:
        return "?";
    }
}