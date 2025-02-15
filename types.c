#include "defs.h"
#include <stdlib.h>

struct CType *__CType_clone(struct CType *ctype)
{
    struct CType *_ctype = new_ctype(ctype->type, ctype->mut);
    _ctype->ptr = ctype->ptr;
    for (int i = 1; i <= ctype->ptr; i++)
        _ctype->offset[i] = ctype->offset[i];
    return _ctype;
}

void __CType_del(struct CType *ctype)
{
    free(ctype);
}

struct CType *new_ctype(Type type, int mut)
{
    struct CType *ctype = (struct CType *)malloc(sizeof(struct CType));
    ctype->type = type;
    ctype->offset[0] = type_size(type);
    ctype->ptr = 0;
    ctype->mut = mut;
    ctype->clone = &__CType_clone;
    ctype->del = &__CType_del;
    return ctype;
}

void point(struct CType *ctype)
{
    ctype->offset[ctype->ptr + 1] = 4;
    ctype->ptr++;
}

void depoint(struct CType *ctype)
{
    ctype->ptr--;
}

void array(struct CType *ctype, int size)
{
    // TODO: dim check
    ctype->offset[ctype->ptr + 1] = ctype->offset[ctype->ptr] * size;
    ctype->ptr++;
    ctype->mut = 0;
}

struct CType *type_cast(struct CType *left, struct CType *right)
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

struct CType *unary_compatible(Kind kind, struct CType *left)
{
    struct CType *ctype = NULL;
    switch (kind)
    {
    case ADDR_N: {
        if (left->ptr)
        {
            ctype = left->clone(left);
            depoint(ctype);
            ctype->mut = 1;
        }
        break;
    }
    case ADDR_OF_N: {
        ctype = left->clone(left);
        point(ctype);
        break;
    }
    case NOT_N: {
        ctype = new_ctype(INT_TYPE, 0);
        break;
    }
    default: {
        ctype = left->clone(left);
        break;
    }
    }
    return ctype;
}

struct CType *binary_compatible(Kind kind, struct CType *left, struct CType *right)
{
    struct CType *ctype = NULL;
    if (kind == COMMA_N)
    {
        ctype = right->clone(right);
    }
    else if (kind == ASSIGN_N)
    {
        if (left->mut && left->ptr == right->ptr)
        {
            ctype = left->clone(left);
        }
    }
    else
    {
        if (left->ptr && right->ptr)
        {
            if (kind >= SUB_N && kind <= L_OR_N)
            {
                ctype = new_ctype(INT_TYPE, 0);
            }
        }
        else if (left->ptr)
        {
            if (kind >= ADD_N && kind <= SUB_N && right->type <= INT_TYPE)
            {
                ctype = left->clone(left);
            }
        }
        else if (right->ptr)
        {
            if (kind >= ADD_N && kind <= SUB_N && left->type <= INT_TYPE)
            {
                ctype = right->clone(right);
            }
        }
        else
        {
            ctype = type_cast(left, right);
        }
    }
    return ctype;
}