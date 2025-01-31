#ifndef TYPES
#define TYPES
#include "token.h"

typedef enum ValueType
{
    UNEXPECTED_VALUE,
    INT_VALUE,
    CHAR_VALUE,
    POINTER_VALUE,
    ARRAY_VALUE,
} ValueType;

char *getValueTypeValue(ValueType valueType);
ValueType tokenTypeToValueType(TokenType tokenType);

typedef struct BaseType
{
    // inhert
    ValueType valueType;
    int offset;
} BaseType;

BaseType *createBaseType(ValueType valueType);
void freeBaseType(BaseType *baseType);

typedef struct PointerType
{
    // inhert
    ValueType valueType;
    int offset;
    // extend
    BaseType *baseType;
    int dim;
} PointerType;

BaseType *createPointerType(BaseType *baseType);
void freePointerType(PointerType *pointerType);

typedef struct ArrayType
{
    // inhert
    ValueType valueType;
    int offset;
    // extend
    BaseType *baseType;
    int size;
} ArrayType;

BaseType *createArrayType(BaseType *baseType, int size);
void freeArrayType(ArrayType *arrayType);

#endif