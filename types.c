#include "types.h"
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
    case BOOL_VALUE:
        return "bool";
    case POINTER_VALUE:
        return "pointer";
    case ARRAY_VALUE:
        return "array";
    default:
        return "?";
    }
}

ValueType tokenTypeToValueType(Token tokenType)
{
    switch (tokenType)
    {
    case INT_T:
        return INT_VALUE;
    case CHAR_T:
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
    case BOOL_VALUE:
        offset = 1;
        break;
    default:
        // ???
        break;
    }
    baseType->offset = offset;
    return baseType;
}

BaseType *cloneBaseType(BaseType *baseType)
{
    switch (baseType->valueType)
    {
    case UNEXPECTED_VALUE:
    case INT_VALUE:
    case CHAR_VALUE:
    case BOOL_VALUE: {
        return createBaseType(baseType->valueType);
    }
    case POINTER_VALUE: {
        return clonePointerType(baseType);
    }
    case ARRAY_VALUE: {
        return cloneArrayType(baseType);
    }
    default: {
        return createBaseType(UNEXPECTED_VALUE);
    }
    }
}

void freeBaseType(void *baseType)
{
    switch (((BaseType *)baseType)->valueType)
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
    if (baseType->valueType == POINTER_VALUE || baseType->valueType == ARRAY_VALUE)
        pointerType->dim = ((PointerType *)baseType)->dim + 1;
    else
        pointerType->dim = 1;
    return (BaseType *)pointerType;
}

BaseType *clonePointerType(BaseType *baseType)
{
    PointerType *refType = (PointerType *)baseType;
    PointerType *cloneType = (PointerType *)malloc(sizeof(PointerType));
    cloneType->valueType = refType->valueType;
    cloneType->offset = refType->offset;
    cloneType->baseType = cloneBaseType(refType->baseType);
    cloneType->dim = refType->dim;
    return (BaseType *)cloneType;
}

void freePointerType(PointerType *pointerType)
{
    if (pointerType->baseType != NULL)
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
    if (baseType->valueType == POINTER_VALUE || baseType->valueType == ARRAY_VALUE)
        arrayType->dim = ((PointerType *)baseType)->dim + 1;
    else
        arrayType->dim = 1;
    return (BaseType *)arrayType;
}

BaseType *cloneArrayType(BaseType *baseType)
{
    ArrayType *refType = (ArrayType *)baseType;
    ArrayType *cloneType = (ArrayType *)malloc(sizeof(ArrayType));
    cloneType->valueType = refType->valueType;
    cloneType->offset = refType->offset;
    cloneType->baseType = cloneBaseType(refType->baseType);
    cloneType->size = refType->size;
    cloneType->dim = refType->dim;
    return (BaseType *)cloneType;
}

void freeArrayType(ArrayType *arrayType)
{
    if (arrayType->baseType != NULL)
        freeBaseType(arrayType->baseType);
    arrayType->baseType = NULL;
    free(arrayType);
}

const int __rules[][3] = {
    {INT_VALUE, INT_VALUE, 0},
    {CHAR_VALUE, CHAR_VALUE, 0},
    {BOOL_VALUE, BOOL_VALUE, 0},
};
const int __ruleSize = 3;

const int __pointerRules[][3] = {
    {POINTER_VALUE, INT_VALUE, 0},
    {INT_VALUE, POINTER_VALUE, 1},
};
const int __pointerRuleSize = 2;

const int __arrayRules[][3] = {
    {ARRAY_VALUE, INT_VALUE, 0},
    {INT_VALUE, ARRAY_VALUE, 1},
};
const int __arrayRuleSize = 2;

const int __assignRules[][3] = {
    {INT_VALUE, INT_VALUE, 0},
    {CHAR_VALUE, CHAR_VALUE, 0},
    {BOOL_VALUE, BOOL_VALUE, 0},
    {POINTER_VALUE, POINTER_VALUE, 0},
};
const int __assignRuleSize = 4;

BaseType *matchRules(const int rules[][3], const int size, BaseType *left, BaseType *right)
{
    for (int i = 0; i < size; i++)
        if (rules[i][0] == left->valueType && rules[i][1] == right->valueType)
            return rules[i][2] ? right : left;
    return NULL;
}

BaseType *computeBinaryOperator(BaseType *left, Kind kind, BaseType *right)
{
    BaseType *baseType;
    switch (kind)
    {
    case ADD_N: {
        baseType = matchRules(__rules, __ruleSize, left, right);
        if (baseType != NULL)
            break;
        baseType = matchRules(__pointerRules, __pointerRuleSize, left, right);
        if (baseType != NULL)
            break;
        baseType = matchRules(__arrayRules, __arrayRuleSize, left, right);
        if (baseType != NULL)
            break;
        return NULL;
    }
    case SUB_N: {
        baseType = matchRules(__rules, __ruleSize, left, right);
        if (baseType != NULL)
            break;
        baseType = matchRules(__pointerRules, __pointerRuleSize, left, right);
        if (baseType != NULL)
            break;
        return NULL;
    }
    case L_AND_N:
    case L_OR_N:
    case MUL_N:
    case DIV_N: {
        baseType = matchRules(__rules, __ruleSize, left, right);
        if (baseType != NULL)
            break;
        return NULL;
    }
    case GT_N:
    case GE_N:
    case LT_N:
    case LE_N:
    case EQ_N:
    case NE_N:
    case AND_N:
    case OR_N: {
        baseType = matchRules(__rules, __ruleSize, left, right);
        if (baseType != NULL)
        {
            baseType = createBaseType(BOOL_VALUE);
            break;
        }
        return NULL;
    }
    case ASSIGN_N: {
        baseType = matchRules(__assignRules, __assignRuleSize, left, right);
        if (baseType != NULL)
            break;
        return NULL;
    }
    case COMMA_N: {
        baseType = right;
        break;
    }
    default:
        return NULL;
    }
    return cloneBaseType(baseType);
}

BaseType *computeUnaryOperator(Kind kind, BaseType *operand)
{
    BaseType *baseType;
    switch (kind)
    {
    case P_ADD_N:
    case P_SUB_N: {
        if (operand->valueType == INT_VALUE)
        {
            baseType = operand;
            break;
        }
        return NULL;
    }
    case NOT_N: {
        baseType = createBaseType(BOOL_VALUE);
        break;
    }
    case ADDR_N: {
        if (operand->valueType == POINTER_VALUE)
        {
            baseType = ((PointerType *)operand)->baseType;
            break;
        }
        else if (operand->valueType == ARRAY_VALUE)
        {
            baseType = ((ArrayType *)operand)->baseType;
            break;
        }
        return NULL;
    }
    case ADDR_OF_N: {
        if (operand->valueType == POINTER_VALUE || operand->valueType == ARRAY_VALUE)
        {
            baseType = createPointerType(operand);
            break;
        }
        return NULL;
    }
    default:
        return NULL;
    }
    return cloneBaseType(baseType);
}
