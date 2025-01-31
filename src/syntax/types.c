#include "syntax/types.h"
#include <stdlib.h>

char *getValueTypeValue(ValueType valueType)
{
    switch (valueType)
    {
    case UNEXPECTED_VALUE:
        return "?";
    case INT_VALUE:
        return "int";
    case CHAR_VALUE:
        return "char";
    case POINTER_VALUE:
        return "pointer";
    case ARRAY_VALUE:
        return "array";
    default:
        return "?";
    }
}

ValueType tokenTypeToValueType(TokenType tokenType)
{
    switch (tokenType)
    {
    case INT_TOKEN:
        return INT_VALUE;
    case CHAR_TOKEN:
        return CHAR_VALUE;
    default:
        return UNEXPECTED_VALUE;
    }
}

BaseType *createBaseType(ValueType valueType)
{
    BaseType *baseType = (BaseType *)malloc(sizeof(BaseType));
    baseType->valueType = valueType;
    int offset = 0;
    switch (valueType)
    {
    case INT_VALUE:
        offset = 4;
        break;
    case CHAR_VALUE:
        offset = 1;
        break;
    default:
        // ???
        break;
    }
    baseType->offset = offset;
    return baseType;
}

void freeBaseType(BaseType *baseType)
{
    switch (baseType->valueType)
    {
    case POINTER_VALUE:
        freePointerType((PointerType *)baseType);
        break;
    case ARRAY_VALUE:
        freeArrayType((ArrayType *)baseType);
        break;
    default:
        break;
    }
    free(baseType);
}

BaseType *createPointerType(BaseType *baseType)
{
    PointerType *pointerType = (PointerType *)malloc(sizeof(PointerType));
    pointerType->valueType = POINTER_VALUE;
    pointerType->offset = 4;
    pointerType->baseType = baseType;
    if (baseType->valueType == POINTER_VALUE)
        pointerType->dim = ((PointerType *)baseType)->dim + 1;
    else
        pointerType->dim = 1;
    return (BaseType *)pointerType;
}

void freePointerType(PointerType *pointerType)
{
    freeBaseType(pointerType->baseType);
    pointerType->baseType = NULL;
    free(pointerType);
}

BaseType *createArrayType(BaseType *baseType, int size)
{
    ArrayType *arrayType = (ArrayType *)malloc(sizeof(ArrayType));
    arrayType->valueType = ARRAY_VALUE;
    arrayType->offset = size * baseType->offset;
    arrayType->baseType = baseType;
    arrayType->size = size;
    return (BaseType *)arrayType;
}

void freeArrayType(ArrayType *arrayType)
{
    freeBaseType(arrayType->baseType);
    arrayType->baseType = NULL;
    free(arrayType);
}
