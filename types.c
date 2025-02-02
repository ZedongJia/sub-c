#include "types.h"
#include <stdlib.h>

char *typeName(Type valueType)
{
    switch (valueType)
    {
    case INT_TYPE:
        return "int";
    case CHAR_TYPE:
        return "char";
    default:
        return "?";
    }
}

Type toType(Token tokenType)
{
    switch (tokenType)
    {
    case INT_T:
        return INT_TYPE;
    case CHAR_T:
        return CHAR_TYPE;
    default:
        return 0;
    }
}

int typeSize(Type type)
{
    switch (type)
    {
    case INT_TYPE:
        return 4;
    case CHAR_TYPE:
        return 1;
    default:
        return 0;
    }
}

CType *createCType(Type type, int modify)
{
    CType *ctype = (CType *)malloc(sizeof(CType));
    ctype->type = type;
    ctype->offset[0] = typeSize(type);
    ctype->ptr = 0;
    ctype->modify = modify;
    return ctype;
}
CType *cloneCType(CType *ctype)
{
    CType *_ctype = createCType(ctype->type, ctype->modify);
    _ctype->ptr = ctype->ptr;
    for (int i = 1; i <= ctype->ptr; i++)
        _ctype->offset[i] = ctype->offset[i];
    return _ctype;
}
void freeCType(CType *ctype)
{
    free(ctype);
}

void point(CType *ctype)
{
    ctype->offset[ctype->ptr + 1] = 4;
    ctype->ptr++;
}

void depoint(CType *ctype)
{
    ctype->ptr--;
}

void array(CType *ctype, int size)
{
    // TODO: dim check
    ctype->offset[ctype->ptr + 1] = ctype->offset[ctype->ptr] * size;
    ctype->ptr++;
}

CType *typeCast(CType *left, CType *right)
{
    if (left->type > INT_TYPE || right->type > INT_TYPE)
    {
        return NULL;
    }
    if (left->type > right->type)
        return left;
    else
        return right;
}

CType *unary_compatible(Kind kind, CType *left)
{
    CType *ctype = NULL;
    switch (kind)
    {
    case ADDR_N: {
        if (left->ptr)
        {
            ctype = cloneCType(left);
            depoint(ctype);
        }
        break;
    }
    case ADDR_OF_N: {
        ctype = cloneCType(left);
        point(ctype);
        break;
    }
    case NOT_N: {
        ctype = createCType(INT_TYPE, 0);
        break;
    }
    default: {
        ctype = cloneCType(left);
        break;
    }
    }
    return ctype;
}

CType *binary_compatible(Kind kind, CType *left, CType *right)
{
    CType *ctype = NULL;
    if (kind == COMMA_N)
    {
        ctype = cloneCType(right);
    }
    else if (kind == ASSIGN_N)
    {
        if (left->modify && left->ptr == right->ptr)
        {
            ctype = cloneCType(left);
        }
    }
    else
    {
        if (left->ptr && right->ptr)
        {
            if (kind >= SUB_N && kind <= L_OR_N)
            {
                ctype = createCType(INT_TYPE, 0);
            }
        }
        else if (left->ptr)
        {
            if (kind >= ADD_N && kind <= SUB_N && right->type <= INT_TYPE)
            {
                ctype = cloneCType(left);
            }
        }
        else if (right->ptr)
        {
            if (kind >= ADD_N && kind <= SUB_N && left->type <= INT_TYPE)
            {
                ctype = cloneCType(right);
            }
        }
        else
        {
            ctype = typeCast(left, right);
        }
    }
    return ctype;
}