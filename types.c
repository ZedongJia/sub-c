#include "defs.h"
#include <math.h>
#include <stdlib.h>

ctype_t *ctype_clone(const ctype_t *ctype)
{
    ctype_t *_ctype = new_ctype(ctype->type, ctype->mut);
    _ctype->ptr = ctype->ptr;
    for (int i = 1; i <= ctype->ptr; i++)
        _ctype->offset[i] = ctype->offset[i];
    return _ctype;
}

void ctype_del(ctype_t *ctype)
{
    free(ctype);
}

ctype_t *new_ctype(type_t type, int mut)
{
    ctype_t *ctype = (ctype_t *)malloc(sizeof(ctype_t));
    ctype->type = type;
    ctype->offset[0] = type_size(type);
    ctype->ptr = 0;
    ctype->mut = mut;
    ctype->clone = &ctype_clone;
    ctype->del = &ctype_del;
    return ctype;
}

int ctype_offset(const ctype_t *ctype)
{
    return ctype->offset[ctype->ptr];
}

int ctype_aligned_offset(const ctype_t *ctype)
{
    return ceil(ctype->offset[ctype->ptr] / 16.0) * 16;
}

int ctype_list_aligned_offset(const list_t *list)
{
    list_iterator_t *iter = list->get_iter(list);
    ctype_t *ctype;
    int offset = 0;
    while (!iter->is_end(iter))
    {
        ctype = (ctype_t *)iter->get(iter);
        offset += ctype->offset[ctype->ptr];
        iter->next(iter);
    }
    iter->del(iter);
    return ceil(offset / 16.0) * 16;
}

void point_to(ctype_t *ctype)
{
    ctype->offset[ctype->ptr + 1] = 4;
    ctype->ptr++;
}

void content_of(ctype_t *ctype)
{
    ctype->ptr--;
}

void array_of(ctype_t *ctype, int size)
{
    // TODO: dim check
    ctype->offset[ctype->ptr + 1] = ctype->offset[ctype->ptr] * size;
    ctype->ptr++;
    ctype->mut = 0;
}

ctype_t *type_widden(ctype_t *left, ctype_t *right)
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

ctype_t *unary_compatible(syntax_kind_t kind, ctype_t *left)
{
    ctype_t *ctype = NULL;
    switch (kind)
    {
    case ADDR_KIND: {
        if (left->ptr)
        {
            ctype = left->clone(left);
            content_of(ctype);
            ctype->mut = 1;
        }
        break;
    }
    case ADDR_OF_KIND: {
        ctype = left->clone(left);
        point_to(ctype);
        break;
    }
    case NOT_KIND: {
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

ctype_t *binary_compatible(syntax_kind_t kind, ctype_t *left, ctype_t *right)
{
    ctype_t *ctype = NULL;
    if (kind == COMMA_KIND)
    {
        ctype = right->clone(right);
    }
    else if (kind == ASSIGN_KIND)
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
            if (kind >= SUB_KIND && kind <= L_OR_KIND)
            {
                ctype = new_ctype(INT_TYPE, 0);
            }
        }
        else if (left->ptr)
        {
            if (kind >= ADD_KIND && kind <= SUB_KIND && right->type <= INT_TYPE)
            {
                ctype = left->clone(left);
            }
        }
        else if (right->ptr)
        {
            if (kind >= ADD_KIND && kind <= SUB_KIND && left->type <= INT_TYPE)
            {
                ctype = right->clone(right);
            }
        }
        else
        {
            ctype = type_widden(left, right);
        }
    }
    return ctype;
}