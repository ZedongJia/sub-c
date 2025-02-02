#include "parser.h"
#include "stmt.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    Parser *parser = createParser();
    while (1)
    {
        printf("[IN]\n");
        ASTNode *root = parseProgram(parser, stdin);
        prettyTree(root);
        freeASTNode(root);
    }
    freeParser(parser);
}