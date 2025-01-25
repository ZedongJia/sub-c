#include "debug.h"
#include "syntax/parser.h"
#include <stdio.h>
int main()
{
    Parser *parser = createParser();
    while (1)
    {
        printf("\033[33;1m[IN]\033[0m\n");
        Node *root = parse(parser, stdin);
        printf("\033[35;1m[OUT]\033[0m\n");
        prettyTree(root);
    }
    freeParser(parser);
}