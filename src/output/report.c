#include "output/color.h"
#include "stdio.h"

void __reportLineAndColumn(int line, int column)
{
    printf("Error at line:%d column:%d: ", line, column);
}

void reportUnclosedString(int line, int column)
{
    setColor(RED);
    __reportLineAndColumn(line, column);
    printf("unclosed string\n");
    clearColor();
}

void reportUnexpectedChar(int line, int column, char ch)
{
    setColor(RED);
    __reportLineAndColumn(line, column);
    printf("unexpected char %c\n", ch);
    clearColor();
}

void reportUnexpectedToken(int line, int column, const char *unexpected, const char *expected)
{
    setColor(RED);
    __reportLineAndColumn(line, column);
    printf("unexpected %s, expect %s\n", unexpected, expected);
    clearColor();
}