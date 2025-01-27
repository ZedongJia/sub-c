#include "output/color.h"
#include "stdio.h"

void reportUnclosedString()
{
    setColor(RED);
    printf("Error: unclosed string\n");
    clearColor();
}

void reportUnexpectedChar(char ch)
{
    setColor(RED);
    printf("Error: unexpected char %c\n", ch);
    clearColor();
}

void reportUnexpectedToken(const char *unexpected, const char *expected)
{
    setColor(RED);
    printf("Error: unexpected %s, expect %s\n", unexpected, expected);
    clearColor();
}