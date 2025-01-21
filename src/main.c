#include "debug.h"
#include "parser.h"
#include <stdio.h>
int main()
{
    struct Parser *parser = createParser();
    while (1)
    {
        printf("> ");
        parse(parser);
        prettyTree(parser->root);
    }
    freeParser(parser);
}