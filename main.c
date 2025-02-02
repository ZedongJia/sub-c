#include "parser.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argv, const char *argc[])
{
    Parser *parser = createParser();
    while (1)
    {
        printf("[IN]\n");
        parseProgram(parser, stdin);
        prettyTree(parser->curr);
    }
    parser->del(parser);
}