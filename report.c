#include "stdio.h"

void reportUnclosedString(int line, int column)
{
    printf("\033[31;1mError at line:%d column:%d: unclosed string\033[0m\n", line, column);
}

void reportUnexpectedChar(int line, int column, char ch)
{
    printf("\033[31;1mError at line:%d column:%d: unexpected char %c\033[0m\n", line, column, ch);
}

void reportUnexpectedToken(int line, int column, const char *unexpected, const char *expected)
{
    printf("\033[31;1mError at line:%d column:%d: unexpected %s, expect %s\033[0m\n", line, column, unexpected,
           expected);
}

void reportVariableUndefined(int line, int column, const char *name)
{
    printf("\033[31;1mError at line:%d column:%d: variable %s has't been defined\033[0m\n", line, column, name);
}

void reportVariabledefined(int line, int column, const char *name)
{
    printf("\033[31;1mError at line:%d column:%d: variable %s has been defined\033[0m\n", line, column, name);
}

void reportBinaryOperatorError(int line, int column, const char *left, const char *op, const char *right)
{
    printf("\033[31;1mError at line:%d column:%d: Not support %s %s %s\033[0m\n", line, column, left, op, right);
}

void reportUnaryOperatorError(int line, int column, const char *op, char *operand)
{
    printf("\033[31;1mError at line:%d column:%d: Not support %s %s\033[0m\n", line, column, op, operand);
}

void reportAccessError(int line, int column, const char *name)
{
    printf("\033[31;1mError at line:%d column:%d: Can't access %s\033[0m\n", line, column, name);
}

void reportCallError(int line, int column, const char *name)
{
    printf("\033[31;1mError at line:%d column:%d: Can't cal %s\033[0m\n", line, column, name);
}