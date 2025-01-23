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
    parser->root = parseProgram(parser, 0);
}
struct Node *parseProgram(struct Parser *parser, int parentPriority)
{
    struct Node **statements = NULL;
    int size = 0;
    struct Token *token;
    struct Node *statement;
    while (1)
    {
        token = peek(parser);
        if (token->tokenType == EndOfLineToken)
        {
            clearLexer(parser->lexer);
            clearParser(parser);
            break;
        }
        if (statements == NULL)
            statements = (struct Node **)malloc(256 * sizeof(struct Node *));
        statements[size] = parseStatement(parser, parentPriority);
        ;
        size++;
    }
    return createProgramNode(statements, size);
}
struct Node *parseCompound(struct Parser *parser, int parentPriority)
{
    struct Node **statements = NULL;
    int size = 0;
    struct Token *token;
    struct Node *statement;
    while (1)
    {
        token = peek(parser);
        if (token->tokenType != LeftBrace)
            break;
        step(parser);
        if (statements == NULL)
            statements = (struct Node **)malloc(256 * sizeof(struct Node *));
        statement = parseStatement(parser, parentPriority);
        token = peek(parser);
        if (token->tokenType == RightBrace)
            step(parser);
        else
            printf("\033[35mError: unexpected %s, expect %s\033[0m\n", getTokenTypeValue(token->tokenType),
                   getTokenTypeValue(RightBrace));
        statements[size] = statement;
        size++;
    }
    return createCompoundNode(statements, size);
}
struct Node *parseStatement(struct Parser *parser, int parentPriority)
{
    struct Node *expression = parseExpression(parser, parentPriority);
    struct Token *token = peek(parser);
    if (token->tokenType == SemiColon)
        step(parser);
    else
        printf("\033[35mError: unexpected %s, expect %s\033[0m\n", getTokenTypeValue(token->tokenType),
               getTokenTypeValue(SemiColon));
    return createStatementNode(expression, SemiColon);
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
        left = createUnaryOperatorExpressionNode(opType, operand);
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
        left = createBinaryOperatorExpressionNode(left, opType, right); // here use opType, so we store opType
        opToken = peek(parser);
        opType = opToken->tokenType;
    }
    return left;
}
struct Node *parsePrimaryExpression(struct Parser *parser)
{
    struct Token *token = peek(parser);
    switch (token->tokenType)
    {
    case LeftBracket: {
        step(parser);
        struct Node *expression = parseExpression(parser, 0);
        token = peek(parser);
        if (token->tokenType == RightBracket)
            step(parser);
        else
            printf("\033[35mError: unexpected %s, expect %s\033[0m\n", getTokenTypeValue(token->tokenType),
                   getTokenTypeValue(RightBracket));
        return expression;
    }
    case IntLiteralToken:
    case IdentifierToken:
    case TrueToken:
    case FalseToken: {
        token = step(parser);
        return createLiteralExpressionNode(token->tokenType, token->value, token->valueLength);
    }
    default: {
        printf("\033[35mError: unexpected %s, expect expression\033[0m\n", getTokenTypeValue(token->tokenType));
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
