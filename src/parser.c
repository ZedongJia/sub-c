#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
struct Parser *createParser()
{
    struct Parser *parser = (struct Parser *)malloc(sizeof(struct Parser));
    parser->lexer = createLexer();
    parser->root = NULL;
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
struct Node *parseUnaryExpression(struct Parser *parser, int parentPriority)
{
    struct Node *expression;
    int priority = getUnaryTokenPriority(peekToken(parser->lexer)->tokenType);
    if (priority != 0 && priority >= parentPriority)
    {
        enum TokenType opType = nextToken(parser->lexer)->tokenType;
        struct Node *operand = parseExpression(parser, priority);
        expression = createUnaryOperatorExpressionNode(opType, operand);
    }
    else
        expression = parsePrimaryExpression(parser);
    return expression;
}
struct Node *parsePrimaryExpression(struct Parser *parser)
{
    switch (peekToken(parser->lexer)->tokenType)
    {
    case LeftBracket: {
        // ( expression )
        nextToken(parser->lexer);
        struct Node *expression = parseExpression(parser, 0);
        if (peekToken(parser->lexer)->tokenType == RightBracket)
            nextToken(parser->lexer);
        else
            printf("\033[35mError: unexpected %s, expect %s\033[0m\n",
                   getTokenTypeValue(peekToken(parser->lexer)->tokenType), getTokenTypeValue(RightBracket));
        return expression;
    }
    case IntLiteralToken:
    case IdentifierToken:
    case TrueToken:
    case FalseToken: {
        struct Token *token = nextToken(parser->lexer);
        return createLiteralExpressionNode(token->tokenType, token->value, token->valueLength);
    }
    default: {
        printf("\033[35mError: unexpected %s, expect expression\033[0m\n",
               getTokenTypeValue(peekToken(parser->lexer)->tokenType));
        return createNode(Err, NULL);
    }
    }
}
struct Node *parseExpression(struct Parser *parser, int parentPriority)
{
    struct Node *left = parseUnaryExpression(parser, parentPriority);
    enum TokenType opType = peekToken(parser->lexer)->tokenType;
    int priority;
    while (1)
    {
        priority = getBinaryTokenPriority(opType);
        if (priority == 0 || priority <= parentPriority)
            break;
        opType = nextToken(parser->lexer)->tokenType;
        struct Node *right = parseExpression(parser, priority);
        left = createBinaryOperatorExpressionNode(left, opType, right); // here use opType, so we store opType
        opType = peekToken(parser->lexer)->tokenType;
    }
    return left;
}
struct Node *parseStatement(struct Parser *parser, int parentPriority)
{
    struct Node *expression = parseExpression(parser, parentPriority);
    if (peekToken(parser->lexer)->tokenType == SemiColon)
        nextToken(parser->lexer);
    else
        printf("\033[35mError: unexpected %s, expect %s\033[0m\n",
               getTokenTypeValue(peekToken(parser->lexer)->tokenType), getTokenTypeValue(SemiColon));
    return createStatementNode(expression, SemiColon);
}
struct Node *parseCompound(struct Parser *parser, int parentPriority)
{
    struct Node **statements = NULL;
    int size = 0;
    while (peekToken(parser->lexer)->tokenType == LeftBrace)
    {
        nextToken(parser->lexer);
        if (statements == NULL)
            statements = (struct Node **)malloc(256 * sizeof(struct Node *));
        statements[size] = parseStatement(parser, parentPriority);
        size++;
        if (peekToken(parser->lexer)->tokenType == RightBrace)
            nextToken(parser->lexer);
        else
            printf("\033[35mError: unexpected %s, expect %s\033[0m\n",
                   getTokenTypeValue(peekToken(parser->lexer)->tokenType), getTokenTypeValue(RightBrace));
    }
    return createCompoundNode(statements, size);
}
struct Node *parseProgram(struct Parser *parser, int parentPriority)
{
    struct Node **statements = NULL;
    int size = 0;
    while (peekToken(parser->lexer)->tokenType != EndOfFileToken)
    {
        if (statements == NULL)
            statements = (struct Node **)malloc(256 * sizeof(struct Node *));
        statements[size] = parseStatement(parser, parentPriority);
        size++;
    }
    nextToken(parser->lexer); // end of line
    clearLexer(parser->lexer);
    return createProgramNode(statements, size);
}
void parse(struct Parser *parser)
{
    parser->root = parseProgram(parser, 0);
}