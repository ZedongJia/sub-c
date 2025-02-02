#ifndef TYPES
#define TYPES
#include "node.h"
typedef enum Type
{
    CHAR_TYPE = 1,
    INT_TYPE,
} Type;

char *typeName(Type type);
Type toType(Token token);

/// @brief Complex Type
typedef struct CType
{
    Type type;
    int offset[16]; // max type recursive = 16
    int ptr;
    int modify;
} CType;

CType *createCType(Type type, int modify);
CType *cloneCType(CType *ctype);
void freeCType(CType *ctype);

/// @brief pointer to this ctype, modify inplace
void point(CType *ctype);

/// @brief depointer to this ctype, modify inplace
void depoint(CType *ctype);

/// @brief array for this ctype, modify inplace
void array(CType *ctype, int size);

/// @brief return widen one
CType *typeCast(CType *left, CType *right);

/// @brief check unary valid, if invalid return `NULL`
CType *unary_compatible(Kind kind, CType *left);

/// @brief check binary valid, if invalid return `NULL`
CType *binary_compatible(Kind kind, CType *left, CType *right);
#endif