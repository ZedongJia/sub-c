#include "token.h"

typedef enum BaseType
{
    UNEXPECTED_TYPE,
    INT_TYPE,
    CHAR_TYPE
} BaseType;

char *getBaseTypeValue(BaseType type);
BaseType tokenTypeToBaseType(TokenType tokenType);