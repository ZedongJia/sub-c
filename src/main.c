#include "debug.h"
#include "parser.h"
#include <stdio.h>
int main()
{
    struct Parser *parser = createParser();
    while (1)
    {
        printf("\033[33;1m>>\033[0m ");
        parse(parser);
        prettyTree(parser->root);
    }
    freeParser(parser);
}