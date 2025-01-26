#include "syntax/parser.h"
#include <stdio.h>
#include <stdlib.h>

Parser *createParser()
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->label = 0;
    return parser;
}

void freeParser(Parser *parser)
{
    free(parser);
}

Node *parsePrimaryExpression(Parser *parser, Lexer *lexer)
{
    peekToken(lexer);
    switch (lexer->postToken->tokenType)
    {
    case LeftBracket: {
        // ( expression )
        matchToken(lexer, LeftBracket);
        Node *expression = parseExpression(parser, lexer, 0);
        matchToken(lexer, RightBracket);
        return expression;
    }
    case TrueToken: {
        nextToken(lexer);
        return createLiteralExpressionNode(IntLiteralToken, "1", 2);
    }
    case FalseToken: {
        nextToken(lexer);
        return createLiteralExpressionNode(IntLiteralToken, "0", 2);
    }
    case IntLiteralToken:
    case StringLiteralToken:
    case IdentifierToken: {
        nextToken(lexer);
        return createLiteralExpressionNode(lexer->currToken->tokenType, lexer->currToken->value,
                                           lexer->currToken->length);
    }
    default: {
        printf("\033[35mError: unexpected %s, expect expression\033[0m\n",
               getTokenTypeValue(lexer->postToken->tokenType));
        return createNode(Err, NULL);
    }
    }
}

Node *parseUnaryExpression(Parser *parser, Lexer *lexer, int parentPriority)
{
    Node *expression;
    peekToken(lexer);
    int priority = getUnaryTokenPriority(lexer->postToken->tokenType);
    if (priority != 0 && priority >= parentPriority)
    {
        // right association
        nextToken(lexer);
        TokenType opType = lexer->currToken->tokenType;
        Node *operand = parseExpression(parser, lexer, priority);
        expression = createUnaryOperatorExpressionNode(opType, operand);
    }
    else
        expression = parsePrimaryExpression(parser, lexer);
    return expression;
}

Node *parseExpression(Parser *parser, Lexer *lexer, int parentPriority)
{
    Node *left = parseUnaryExpression(parser, lexer, parentPriority);
    peekToken(lexer);
    TokenType opType = lexer->postToken->tokenType;
    int priority = getBinaryTokenPriority(opType);
    int association = getAssociation(opType);
    while ((priority != 0) && (association ? priority >= parentPriority : priority > parentPriority))
    {
        nextToken(lexer);
        opType = lexer->currToken->tokenType;
        Node *right = parseExpression(parser, lexer, priority);
        left = createBinaryOperatorExpressionNode(left, opType, right); // here use opType, so we store opType
        // peek next token
        peekToken(lexer);
        opType = lexer->postToken->tokenType;
        priority = getBinaryTokenPriority(opType);
        association = getAssociation(opType);
    }
    return left;
}

Node *parseStatement(Parser *parser, Lexer *lexer)
{
    Node *statement;
    peekToken(lexer);
    switch (lexer->postToken->tokenType)
    {
    case IntToken:
    case CharToken: {
        statement = parseDeclarationStatement(parser, lexer);
        break;
    }
    case IfToken: {
        statement = parseIfStatement(parser, lexer);
        break;
    }
    case ForToken: {
        statement = parseForStatement(parser, lexer);
        break;
    }
    case WhileToken: {
        statement = parseWhileStatement(parser, lexer);
        break;
    }
    default: {
        statement = createStatementNode(parseExpression(parser, lexer, 0));
        matchToken(lexer, SemiColon);
        break;
    }
    }
    return statement;
}

Node *parseDeclarationStatement(Parser *parser, Lexer *lexer)
{
    nextToken(lexer);
    Node *type = createTypeExpressionNode(lexer->currToken->tokenType);
    Node *expression = parseExpression(parser, lexer, 0);
    matchToken(lexer, SemiColon);
    return createDeclarationNode(type, expression);
}

Node *parseIfStatement(Parser *parser, Lexer *lexer)
{
    Node *scopeNode = createScopeNode(1);
    matchToken(lexer, IfToken);
    matchToken(lexer, LeftBracket);
    int trueEndLabel = parser->label++;
    appendNodeToScope(scopeNode, createJumpIfFalseStatementNode(parseExpression(parser, lexer, 0), trueEndLabel));
    matchToken(lexer, RightBracket);
    appendNodeToScope(scopeNode, parseStatements(parser, lexer, 0));
    peekToken(lexer);
    if (lexer->postToken->tokenType == ElseToken)
    {
        int falseEndLabel = parser->label++;
        appendNodeToScope(scopeNode, createJumpStatementNode(falseEndLabel));
        appendNodeToScope(scopeNode, createLabelStatementNode(trueEndLabel));
        appendNodeToScope(scopeNode, parseElseStatement(parser, lexer));
        appendNodeToScope(scopeNode, createLabelStatementNode(falseEndLabel));
    }
    else
    {
        appendNodeToScope(scopeNode, createLabelStatementNode(trueEndLabel));
    }
    return scopeNode;
}

Node *parseElseStatement(Parser *parser, Lexer *lexer)
{
    matchToken(lexer, ElseToken);
    return parseStatements(parser, lexer, 0);
}

Node *parseForStatement(Parser *parser, Lexer *lexer)
{
    Node *scopeNode = createScopeNode(0);
    matchToken(lexer, ForToken);
    matchToken(lexer, LeftBracket);
    appendNodeToScope(scopeNode, parseStatement(parser, lexer));
    int ForStartLabel = parser->label++;
    appendNodeToScope(scopeNode, createLabelStatementNode(ForStartLabel));
    int ForEndLabel = parser->label++;
    appendNodeToScope(scopeNode, createJumpIfFalseStatementNode(parseExpression(parser, lexer, 0), ForEndLabel));
    matchToken(lexer, SemiColon);
    Node *expression = parseExpression(parser, lexer, 0);
    matchToken(lexer, RightBracket);
    appendNodeToScope(scopeNode, parseStatements(parser, lexer, 0));
    appendNodeToScope(scopeNode, expression);
    appendNodeToScope(scopeNode, createJumpStatementNode(ForStartLabel));
    appendNodeToScope(scopeNode, createLabelStatementNode(ForEndLabel));
    return scopeNode;
}

Node *parseWhileStatement(Parser *parser, Lexer *lexer)
{
    Node *scopeNode = createScopeNode(0);
    matchToken(lexer, WhileToken);
    matchToken(lexer, LeftBracket);
    int whileStartLabel = parser->label++;
    appendNodeToScope(scopeNode, createLabelStatementNode(whileStartLabel));
    int whileEndLabel = parser->label++;
    appendNodeToScope(scopeNode, createJumpIfFalseStatementNode(parseExpression(parser, lexer, 0), whileEndLabel));
    matchToken(lexer, RightBracket);
    appendNodeToScope(scopeNode, parseStatements(parser, lexer, 0));
    appendNodeToScope(scopeNode, createJumpStatementNode(whileStartLabel));
    appendNodeToScope(scopeNode, createLabelStatementNode(whileEndLabel));
    return scopeNode;
}

Node *parseStatements(Parser *parser, Lexer *lexer, int isGlobal)
{
    Node *scopeNode = createScopeNode(0);
    if (!isGlobal)
        matchToken(lexer, LeftBrace);
    peekToken(lexer);
    while ((lexer->postToken->tokenType != EndOfFileToken) &&
           ((isGlobal) || (!isGlobal && lexer->postToken->tokenType != RightBrace)))
    {
        appendNodeToScope(scopeNode, parseStatement(parser, lexer));
        peekToken(lexer);
    }
    isGlobal ? matchToken(lexer, EndOfFileToken) : matchToken(lexer, RightBrace);
    return scopeNode;
}

Node *parse(Parser *parser, FILE *file)
{
    Lexer *lexer = createLexer(file);
    Node *root = parseStatements(parser, lexer, 1);
    freeLexer(lexer);
    return root;
}