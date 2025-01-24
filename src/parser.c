#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
Parser *createParser()
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->lexer = createLexer();
    parser->root = NULL;
    return parser;
}
void freeParser(Parser *parser)
{
    free(parser->lexer);
    parser->lexer = NULL;
    if (parser->root != NULL)
        freeNode(parser->root);
    parser->root = NULL;
    free(parser);
}
Node *parseUnaryExpression(Parser *parser, int parentPriority)
{
    Node *expression;
    int priority = getUnaryTokenPriority(peekToken(parser->lexer)->tokenType);
    if (priority != 0 && priority >= parentPriority)
    {
        // right association
        TokenType opType = nextToken(parser->lexer)->tokenType;
        Node *operand = parseExpression(parser, priority);
        expression = createUnaryOperatorExpressionNode(opType, operand);
    }
    else
        expression = parsePrimaryExpression(parser);
    return expression;
}
Node *parsePrimaryExpression(Parser *parser)
{
    switch (peekToken(parser->lexer)->tokenType)
    {
    case LeftBracket: {
        // ( expression )
        matchToken(parser->lexer, LeftBracket);
        Node *expression = parseExpression(parser, 0);
        matchToken(parser->lexer, RightBracket);
        return expression;
    }
    case TrueToken:
    case FalseToken: {
        const Token *token = nextToken(parser->lexer);
        return createKeywordExpressionNode(token->tokenType);
    }
    case IntLiteralToken:
    case IdentifierToken: {
        const Token *token = nextToken(parser->lexer);
        return createLiteralExpressionNode(token->tokenType, token->value, token->valueLength);
    }
    default: {
        printf("\033[35mError: unexpected %s, expect expression\033[0m\n",
               getTokenTypeValue(peekToken(parser->lexer)->tokenType));
        return createNode(Err, NULL);
    }
    }
}
Node *parseExpression(Parser *parser, int parentPriority)
{
    Node *left = parseUnaryExpression(parser, parentPriority);
    TokenType opType = peekToken(parser->lexer)->tokenType;
    int priority = getBinaryTokenPriority(opType);
    int association = getAssociation(opType);
    while ((priority != 0) && (association ? priority >= parentPriority : priority > parentPriority))
    {
        opType = nextToken(parser->lexer)->tokenType;
        Node *right = parseExpression(parser, priority);
        left = createBinaryOperatorExpressionNode(left, opType, right); // here use opType, so we store opType
        // peek next token
        opType = peekToken(parser->lexer)->tokenType;
        priority = getBinaryTokenPriority(opType);
        association = getAssociation(opType);
    }
    return left;
}
Node *parseStatement(Parser *parser, int parentPriority)
{
    Node *expression = parseExpression(parser, parentPriority);
    matchToken(parser->lexer, SemiColon);
    return createStatementNode(expression, SemiColon);
}
Node *parseDeclarationStatement(Parser *parser, int parentPriority)
{
    Node *type = createKeywordExpressionNode(nextToken(parser->lexer)->tokenType);
    Node *expression = parseExpression(parser, parentPriority);
    matchToken(parser->lexer, SemiColon);
    return createDeclarationNode(type, expression, SemiColon);
}
Node *parseCompound(Parser *parser, int parentPriority)
{
    Node **statements = NULL;
    int size = 0;
    Node *statement;
    matchToken(parser->lexer, LeftBrace);
    while (peekToken(parser->lexer)->tokenType != LeftBrace || peekToken(parser->lexer)->tokenType != EndOfFileToken)
    {
        if (statements == NULL)
            statements = (Node **)malloc(256 * sizeof(Node *));
        switch (peekToken(parser->lexer)->tokenType)
        {
        case IntToken:
            statement = parseDeclarationStatement(parser, parentPriority);
            break;
        default:
            statement = parseStatement(parser, parentPriority);
            break;
        }
        statements[size] = statement;
        size++;
    }
    matchToken(parser->lexer, RightBrace);
    return createCompoundNode(LeftBrace, statements, size, RightBrace);
}
Node *parseProgram(Parser *parser, int parentPriority)
{
    Node **statements = NULL;
    int size = 0;
    Node *statement;
    while (peekToken(parser->lexer)->tokenType != EndOfFileToken)
    {
        if (statements == NULL)
            statements = (Node **)malloc(256 * sizeof(Node *));
        switch (peekToken(parser->lexer)->tokenType)
        {
        case IntToken:
            statement = parseDeclarationStatement(parser, parentPriority);
            break;
        default:
            statement = parseStatement(parser, parentPriority);
            break;
        }
        statements[size] = statement;
        size++;
    }
    matchToken(parser->lexer, EndOfFileToken);
    return createProgramNode(statements, size, EndOfFileToken);
}
void parse(Parser *parser)
{
    parser->root = parseProgram(parser, 0);
    clearLexer(parser->lexer);
}