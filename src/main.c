#include "output/color.h"
#include "output/tree.h"
#include "syntax/parser.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    Parser *parser = createParser();
    while (1)
    {
        setColor(BLACK);
        printf("[IN]\n");
        clearColor();
        Node *root = parse(parser, stdin);
        setColor(BLACK);
        printf("[OUT]\n");
        clearColor();
        prettyTree(root);
        freeNode(root);
    }
    freeParser(parser);
}