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
    case LEFT_PARENTHESIS: {
        // ( expression )
        matchToken(lexer, LEFT_PARENTHESIS);
        Node *expression = parseExpression(parser, lexer, 0);
        matchToken(lexer, RIGHT_PARENTHESIS);
        return expression;
    }
    case TRUE_TOKEN: {
        nextToken(lexer);
        return createLiteral(INT_LITERAL_TOKEN, "1", 2);
    }
    case FALSE_TOKEN: {
        nextToken(lexer);
        return createLiteral(INT_LITERAL_TOKEN, "0", 2);
    }
    case INT_LITERAL_TOKEN:
    case STRING_LITERAL_TOKEN:
    case IDENTIFIER_TOKEN: {
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
    while (type == LEFT_BRACKET)
    {
        nextToken(lexer);
        right = parseExpression(parser, lexer, 0);
        matchToken(lexer, RIGHT_BRACKET);
        base = createBinaryOperator(base, PLUS_TOKEN, right);
        base = createUnaryOperator(STAR_TOKEN, base);
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
    case INT_TOKEN:
    case CHAR_TOKEN: {
        statement = parseDeclarationStatement(parser, lexer);
        break;
    }
    case IF_TOKEN: {
        statement = parseIfStatement(parser, lexer);
        break;
    }
    case FOR_TOKEN: {
        statement = parseForStatement(parser, lexer);
        break;
    }
    case WHILE_TOKEN: {
        statement = parseWhileStatement(parser, lexer);
        break;
    }
    default: {
        statement = parseExpression(parser, lexer, 0);
        matchToken(lexer, SEMI_COLON_TOKEN);
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
    matchToken(lexer, SEMI_COLON_TOKEN);
    return createDeclaration(type, expression);
}

Node *parseIfStatement(Parser *parser, Lexer *lexer)
{
    Node *scope = createScope(1);
    List *list = ((Scope *)scope)->list;
    matchToken(lexer, IF_TOKEN);
    matchToken(lexer, LEFT_PARENTHESIS);
    int trueEndLabelNumber = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), trueEndLabelNumber));
    matchToken(lexer, RIGHT_PARENTHESIS);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LEFT_BRACE)
        appendToList(list, parseStatements(parser, lexer, 0));
    else
        appendToList(list, parseStatement(parser, lexer));
    peekToken(lexer);
    if (lexer->postToken->tokenType == ELSE_TOKEN)
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
    matchToken(lexer, ELSE_TOKEN);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LEFT_BRACE)
        return parseStatements(parser, lexer, 0);
    else
        return parseStatement(parser, lexer);
}

Node *parseForStatement(Parser *parser, Lexer *lexer)
{
    Node *scope = createScope(0);
    List *list = ((Scope *)scope)->list;
    matchToken(lexer, FOR_TOKEN);
    matchToken(lexer, LEFT_PARENTHESIS);
    appendToList(list, parseStatement(parser, lexer));
    int ForStartLabelNumber = parser->labelNumber++;
    appendToList(list, createLabel(ForStartLabelNumber));
    int ForEndLabelNumber = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), ForEndLabelNumber));
    matchToken(lexer, SEMI_COLON_TOKEN);
    Node *expression = parseExpression(parser, lexer, 0);
    matchToken(lexer, RIGHT_PARENTHESIS);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LEFT_BRACE)
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
    matchToken(lexer, WHILE_TOKEN);
    matchToken(lexer, LEFT_PARENTHESIS);
    int whileStartLabelNumber = parser->labelNumber++;
    appendToList(list, createLabel(whileStartLabelNumber));
    int whileEndLabelNumber = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), whileEndLabelNumber));
    matchToken(lexer, RIGHT_PARENTHESIS);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LEFT_BRACE)
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
        matchToken(lexer, LEFT_BRACE);
    peekToken(lexer);
    while ((lexer->postToken->tokenType != END_OF_FILE_TOKEN) &&
           ((isGlobal) || (!isGlobal && lexer->postToken->tokenType != RIGHT_BRACE)))
    {
        appendToList(list, parseStatement(parser, lexer));
        peekToken(lexer);
    }
    isGlobal ? matchToken(lexer, END_OF_FILE_TOKEN) : matchToken(lexer, RIGHT_BRACE);
    return scope;
}

Node *parse(Parser *parser, FILE *file)
{
    Lexer *lexer = createLexer(file);
    Node *root = parseStatements(parser, lexer, 1);
    freeLexer(lexer);
    return root;
}