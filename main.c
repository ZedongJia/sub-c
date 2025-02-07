#include "defs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argv, const char *argc[])
{
    struct Parser *parser = new_parser(stdin);
    while (1)
    {
        printf("[IN]\n");
        parse_program(parser);
        __debug_pretty_tree(parser->curr);
    }
    parser->del(parser);
}