#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
struct Parser *createParser()
{
    struct Parser *parser = (struct Parser *)malloc(sizeof(struct Parser));
    parser->lexer = createLexer();
    parser->root = NULL;
    parser->currentToken = NULL;
    parser->peekToken = NULL;
    return parser;
}
void freeParser(struct Parser *parser)
{
    free(parser->lexer);
    parser->lexer = NULL;
    if (parser->root != NULL)
        freeNode(parser->root);
    parser->root = NULL;
    free(parser);
}

void clearParser(struct Parser *parser)
{
    if (parser->currentToken != NULL)
    {
        freeToken(parser->currentToken);
        parser->currentToken = NULL;
    }
    if (parser->peekToken != NULL)
    {
        freeToken(parser->peekToken);
        parser->peekToken = NULL;
    }
}

void parse(struct Parser *parser)
{
    clearLexer(parser->lexer);
    clearParser(parser);
    parser->root = parseExpression(parser, 0);
}

struct Node *parseExpression(struct Parser *parser, int parentPriority)
{
    struct Node *left;
    struct Token *token = peek(parser);
    int priority = getUnaryTokenPriority(token->tokenType);
    if (priority != 0 && priority >= parentPriority)
    {
        enum TokenType opType = token->tokenType;
        step(parser);
        struct Node *operand = parseExpression(parser, priority);
        left = createUnaryOperatorNode(opType, operand);
    }
    else
    {
        left = parsePrimaryExpression(parser);
    }

    struct Token *opToken = peek(parser);
    enum TokenType opType = opToken->tokenType;
    while (1)
    {
        int priority = getBinaryTokenPriority(opType);
        if (priority == 0 || priority <= parentPriority)
            break;
        step(parser); // to this peekToken
        struct Node *right = parseExpression(parser, priority);
        left = createBinaryOperatorNode(left, opType, right); // here use opType, so we store opType
        opToken = peek(parser);
        opType = opToken->tokenType;
    }
    return left;
}

struct Node *parsePrimaryExpression(struct Parser *parser)
{
    struct Token *token = step(parser);
    switch (token->tokenType)
    {
    case LeftBracket: {
        struct Node *node = parseExpression(parser, 0);
        step(parser);
        return node;
    }
    case LiteralToken: {
        return createLiteralNode(token->value, token->valueLength);
    }
    default: {
        printf("\033[35mError: unexpected %s, expect %s\033[0m\n", getTokenTypeValue(token->tokenType),
               getTokenTypeValue(LiteralToken));
        return createNode(Err, NULL);
    }
    }
}

struct Token *step(struct Parser *parser)
{
    if (parser->currentToken != NULL)
    {
        // clear currentToken
        freeToken(parser->currentToken);
        parser->currentToken = NULL;
    }
    if (parser->peekToken != NULL)
    {
        // load from peekToken
        parser->currentToken = parser->peekToken;
        parser->peekToken = NULL;
    }
    else
    {
        // load new Token
        parser->currentToken = nextToken(parser->lexer);
    }
    return parser->currentToken;
}

struct Token *peek(struct Parser *parser)
{
    if (parser->peekToken == NULL)
        parser->peekToken = nextToken(parser->lexer);
    return parser->peekToken;
}
