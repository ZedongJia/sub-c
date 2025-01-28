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