#include "syntax/parser.h"
#include <stdio.h>
#include <stdlib.h>

Parser *createParser()
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->labelNumber = 0;
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
    case LeftParenthesis: {
        // ( expression )
        matchToken(lexer, LeftParenthesis);
        Node *expression = parseExpression(parser, lexer, 0);
        matchToken(lexer, RightParenthesis);
        return expression;
    }
    case TrueToken: {
        nextToken(lexer);
        return createLiteral(IntLiteralToken, "1", 2);
    }
    case FalseToken: {
        nextToken(lexer);
        return createLiteral(IntLiteralToken, "0", 2);
    }
    case IntLiteralToken:
    case StringLiteralToken:
    case IdentifierToken: {
        nextToken(lexer);
        return createLiteral(lexer->currToken->tokenType, lexer->currToken->value, lexer->currToken->length);
    }
    default: {
        printf("\033[35mError: unexpected %s, expect expression\033[0m\n",
               getTokenTypeValue(lexer->postToken->tokenType));
        return createNode(UNEXPECTED);
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
        TokenType type = lexer->currToken->tokenType;
        Node *operand = parseExpression(parser, lexer, priority);
        expression = createUnaryOperator(type, operand);
    }
    else
        expression = parsePrimaryExpression(parser, lexer);
    return expression;
}

Node *parseAccessExpression(Node *base, Parser *parser, Lexer *lexer)
{
    Node *right = NULL;
    TokenType type;
    peekToken(lexer);
    type = lexer->postToken->tokenType;
    while (type == LeftBracket)
    {
        nextToken(lexer);
        right = parseExpression(parser, lexer, 0);
        matchToken(lexer, RightBracket);
        base = createBinaryOperator(base, PlusToken, right);
        base = createUnaryOperator(StarToken, base);
        peekToken(lexer);
        type = lexer->postToken->tokenType;
    }
    return base;
}

Node *parseBinaryExpression(Node *base, Parser *parser, Lexer *lexer, int parentPriority)
{
    Node *right = NULL;
    TokenType type;
    peekToken(lexer);
    type = lexer->postToken->tokenType;
    int priority = getBinaryTokenPriority(type);
    int association = getAssociation(type);
    while ((priority != 0) && (association ? priority >= parentPriority : priority > parentPriority))
    {
        nextToken(lexer);
        type = lexer->currToken->tokenType;
        right = parseExpression(parser, lexer, priority);
        base = createBinaryOperator(base, type, right); // here use type, so we store type
        // peek next token
        peekToken(lexer);
        type = lexer->postToken->tokenType;
        priority = getBinaryTokenPriority(type);
        association = getAssociation(type);
    }
    return base;
}

Node *parseExpression(Parser *parser, Lexer *lexer, int parentPriority)
{
    // op expression
    Node *expression = parseUnaryExpression(parser, lexer, parentPriority);
    expression = parseAccessExpression(expression, parser, lexer);
    expression = parseBinaryExpression(expression, parser, lexer, parentPriority);
    return expression;
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
        statement = parseExpression(parser, lexer, 0);
        matchToken(lexer, SemiColon);
        break;
    }
    }
    return statement;
}

Node *parseDeclarationStatement(Parser *parser, Lexer *lexer)
{
    nextToken(lexer);
    Node *type = createType(lexer->currToken->tokenType);
    Node *expression = parseExpression(parser, lexer, 0);
    matchToken(lexer, SemiColon);
    return createDeclaration(type, expression);
}

Node *parseIfStatement(Parser *parser, Lexer *lexer)
{
    Node *scope = createScope(1);
    List *list = ((Scope *)scope)->list;
    matchToken(lexer, IfToken);
    matchToken(lexer, LeftParenthesis);
    int trueEndLabelNumber = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), trueEndLabelNumber));
    matchToken(lexer, RightParenthesis);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LeftBrace)
        appendToList(list, parseStatements(parser, lexer, 0));
    else
        appendToList(list, parseStatement(parser, lexer));
    peekToken(lexer);
    if (lexer->postToken->tokenType == ElseToken)
    {
        int falseEndLabelNumber = parser->labelNumber++;
        appendToList(list, createJump(falseEndLabelNumber));
        appendToList(list, createLabel(trueEndLabelNumber));
        appendToList(list, parseElseStatement(parser, lexer));
        appendToList(list, createLabel(falseEndLabelNumber));
    }
    else
    {
        appendToList(list, createLabel(trueEndLabelNumber));
    }
    return scope;
}

Node *parseElseStatement(Parser *parser, Lexer *lexer)
{
    matchToken(lexer, ElseToken);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LeftBrace)
        return parseStatements(parser, lexer, 0);
    else
        return parseStatement(parser, lexer);
}

Node *parseForStatement(Parser *parser, Lexer *lexer)
{
    Node *scope = createScope(0);
    List *list = ((Scope *)scope)->list;
    matchToken(lexer, ForToken);
    matchToken(lexer, LeftParenthesis);
    appendToList(list, parseStatement(parser, lexer));
    int ForStartLabelNumber = parser->labelNumber++;
    appendToList(list, createLabel(ForStartLabelNumber));
    int ForEndLabelNumber = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), ForEndLabelNumber));
    matchToken(lexer, SemiColon);
    Node *expression = parseExpression(parser, lexer, 0);
    matchToken(lexer, RightParenthesis);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LeftBrace)
        appendToList(list, parseStatements(parser, lexer, 0));
    else
        appendToList(list, parseStatement(parser, lexer));
    appendToList(list, expression);
    appendToList(list, createJump(ForStartLabelNumber));
    appendToList(list, createLabel(ForEndLabelNumber));
    return scope;
}

Node *parseWhileStatement(Parser *parser, Lexer *lexer)
{
    Node *scope = createScope(0);
    List *list = ((Scope *)scope)->list;
    matchToken(lexer, WhileToken);
    matchToken(lexer, LeftParenthesis);
    int whileStartLabelNumber = parser->labelNumber++;
    appendToList(list, createLabel(whileStartLabelNumber));
    int whileEndLabelNumber = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), whileEndLabelNumber));
    matchToken(lexer, RightParenthesis);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LeftBrace)
        appendToList(list, parseStatements(parser, lexer, 0));
    else
        appendToList(list, parseStatement(parser, lexer));
    appendToList(list, createJump(whileStartLabelNumber));
    appendToList(list, createLabel(whileEndLabelNumber));
    return scope;
}

Node *parseStatements(Parser *parser, Lexer *lexer, int isGlobal)
{
    Node *scope = createScope(0);
    List *list = ((Scope *)scope)->list;
    if (!isGlobal)
        matchToken(lexer, LeftBrace);
    peekToken(lexer);
    while ((lexer->postToken->tokenType != EndOfFileToken) &&
           ((isGlobal) || (!isGlobal && lexer->postToken->tokenType != RightBrace)))
    {
        appendToList(list, parseStatement(parser, lexer));
        peekToken(lexer);
    }
    isGlobal ? matchToken(lexer, EndOfFileToken) : matchToken(lexer, RightBrace);
    return scope;
}

Node *parse(Parser *parser, FILE *file)
{
    Lexer *lexer = createLexer(file);
    Node *root = parseStatements(parser, lexer, 1);
    freeLexer(lexer);
    return root;
}