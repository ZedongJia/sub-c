#ifndef REPORT
#define REPORT
#include "types.h"
#include "tree.h"
void reportUnclosedString(int line, int column);
void reportUnexpectedChar(int line, int column, char ch);
void reportUnexpectedToken(int line, int column, const char *unexpected, const char *expected);
void reportVariableUndefined(int line, int column, const char *name);
void reportVariabledefined(int line, int column, const char *name);
void reportBinaryOperatorError(int line, int column, const char *left, const char *op, const char *right);
void reportUnaryOperatorError(int line, int column, const char *op, char *operand);
void reportAccessError(int line, int column, const char *name);
void reportCallError(int line, int column, const char *name);
void reportUnaryInCompatible(int line, int column, const CType *t, const char *op);
void reportBinaryInCompatible(int line, int column, const CType *t1, const char *op, const CType *t2);
#endif