#include "list.h"
#include "node.h"

#ifndef TYPES
#define TYPES
typedef enum ValueType
{
    UNEXPECTED_VALUE,
    INT_VALUE,
    CHAR_VALUE,
    BOOL_VALUE,
    POINTER_VALUE,
    ARRAY_VALUE,
} ValueType;

char *getValueTypeValue(ValueType valueType);
ValueType tokenTypeToValueType(Token tokenType);

typedef struct BaseType
{
    // inhert
    ValueType valueType;
    int offset;
} BaseType;

BaseType *createBaseType(ValueType valueType);
BaseType *cloneBaseType(BaseType *baseType);
void freeBaseType(void *baseType);

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
BaseType *clonePointerType(BaseType *baseType);
void freePointerType(PointerType *pointerType);

typedef struct ArrayType
{
    // inhert
    ValueType valueType;
    int offset;
    // extend
    BaseType *baseType;
    int size;
    int dim;
} ArrayType;

BaseType *createArrayType(BaseType *baseType, int size);
BaseType *cloneArrayType(BaseType *baseType);
void freeArrayType(ArrayType *arrayType);

BaseType *computeBinaryOperator(BaseType *left, Kind kind, BaseType *right);
BaseType *computeUnaryOperator(Kind kind, BaseType *operand);
#endif