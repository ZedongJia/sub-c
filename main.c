#include "parser.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    Parser *parser = createParser();
    while (1)
    {
        printf("[IN]\n");
        ASTNode *root = parse(parser, stdin);
        prettyTree(root);
        freeASTNode(root);
    }
    freeParser(parser);
}