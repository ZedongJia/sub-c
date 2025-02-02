#ifndef TYPES
#define TYPES
#include "node.h"
typedef enum Type
{
    BOOL_VALUE = 1,
    CHAR_VALUE,
    INT_VALUE,
} Type;

char *typeName(Type type);
Type toType(Token token);

/// @brief Complex Type
typedef struct CType
{
    Type type;
    int offset[16]; // max type recursive = 16
    int ptr;
    int dim;
} CType;

CType *createCType(Type type);
void freeCType(CType *ctype);

/// @brief pointer to this ctype, modify inplace
void point(CType *ctype);
/// @brief array for this ctype, modify inplace
void array(CType *ctype, int size);
#endif