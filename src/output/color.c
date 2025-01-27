#include "output/color.h"
#include "stdio.h"

void setColor(Color color)
{
    printf("\033[%d;1m", color);
}
void clearColor()
{
    printf("\033[0;1m");
}