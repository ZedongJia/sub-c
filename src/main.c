#include "debug.h"
#include "syntax/parser.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    Parser *parser = createParser();
    while (1)
    {
        printf("\033[30;1m[IN]\033[0m\n");
        Node *root = parse(parser, stdin);
        printf("\033[30;1m[OUT]\033[0m\n");
        prettyTree(root);
        freeNode(root);
    }
    freeParser(parser);
}