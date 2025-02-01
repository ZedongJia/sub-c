#include "output/tree.h"
#include "syntax/parser.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    Parser *parser = createParser();
    while (1)
    {
        printf("[IN]\n");
        Node *root = parse(parser, stdin);
        prettyTree(root);
        freeNode(root);
    }
    freeParser(parser);
}