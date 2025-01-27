#include "syntax/types.h"

char *getBaseTypeValue(BaseType type)
{
    switch (type)
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

BaseType tokenTypeToBaseType(TokenType tokenType)
{
    switch (tokenType)
    {
    case INT_TOKEN:
        return INT_TYPE;
    case CHAR_TOKEN:
        return CHAR_TYPE;
    default:
        return UNEXPECTED_TYPE;
    }
}
