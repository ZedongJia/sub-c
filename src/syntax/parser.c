#include "syntax/parser.h"
#include <stdio.h>
#include <stdlib.h>

Parser *createParser()
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->labelNumber = 0;
    parser->currScope = NULL;
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
    case LEFT_BRACE: {
        // { expression }
        matchToken(lexer, LEFT_BRACE);
        Node *expression = parseExpression(parser, lexer, 0);
        matchToken(lexer, RIGHT_BRACE);
        return expression;
    }
    case TRUE_TOKEN:
    case FALSE_TOKEN: {
        nextToken(lexer);
        return createLiteral(createBaseType(BOOL_VALUE), lexer->currToken->tokenType, lexer->currToken->value);
    }
    case INT_LITERAL_TOKEN: {
        nextToken(lexer);
        return createLiteral(createBaseType(INT_VALUE), lexer->currToken->tokenType, lexer->currToken->value);
    }
    case STRING_LITERAL_TOKEN: {
        nextToken(lexer);
        return createLiteral(createPointerType(createBaseType(CHAR_VALUE)), lexer->currToken->tokenType,
                             lexer->currToken->value);
    }
    case IDENTIFIER_TOKEN: {
        nextToken(lexer);
        Scope *p = parser->currScope;
        while (p != NULL)
        {
            int index = tryLookUp(&p->table, lexer->currToken->value);
            if (index != -1)
                return createLiteral(p->table.variables[index].baseType, lexer->currToken->tokenType,
                                     lexer->currToken->value);
            p = p->parentScope;
        }
        reportVariableUndefined(lexer->currToken->line, lexer->currToken->column, lexer->currToken->value);
        return createLiteral(createBaseType(UNEXPECTED_VALUE), lexer->currToken->tokenType, lexer->currToken->value);
    }
    default: {
        reportUnexpectedToken(lexer->postToken->line, lexer->postToken->column,
                              getTokenTypeValue(lexer->postToken->tokenType), "expression");
        return createNode(UNEXPECTED_NODE);
    }
    }
}

Node *parseUnaryExpression(Parser *parser, Lexer *lexer, int parentPriority)
{
    peekToken(lexer);
    Node *expression;
    int priority = getUnaryTokenPriority(lexer->postToken->tokenType);
    if (priority != 0 && priority >= parentPriority)
    {
        // right association
        nextToken(lexer);
        int line = lexer->currToken->line;
        int column = lexer->currToken->column;
        TokenType tokenType = lexer->currToken->tokenType;
        Node *operand = parseExpression(parser, lexer, priority);
        BaseType *baseType = computeUnaryOperator(STAR_TOKEN, ((ExpressionView *)operand)->baseType);
        if (baseType == NULL)
        {
            reportUnaryOperatorError(line, column, getTokenTypeValue(tokenType),
                                     getValueTypeValue(((ExpressionView *)operand)->baseType->valueType));
            baseType = ((ExpressionView *)operand)->baseType;
        }
        expression = createUnaryOperator(baseType, tokenType, operand);
    }
    else
        expression = parsePrimaryExpression(parser, lexer);
    return expression;
}

Node *parseAccessExpression(Node *base, Parser *parser, Lexer *lexer)
{
    Node *right = NULL;
    TokenType tokenType;
    int isDone = 0;
    while (!isDone)
    {
        peekToken(lexer);
        tokenType = lexer->postToken->tokenType;
        switch (tokenType)
        {
        case LEFT_BRACKET: {
            // []
            nextToken(lexer);
            int line = lexer->currToken->line;
            int column = lexer->currToken->column;
            right = parseExpression(parser, lexer, 0);
            matchToken(lexer, RIGHT_BRACKET);
            BaseType *baseType = computeBinaryOperator(((ExpressionView *)base)->baseType, PLUS_TOKEN,
                                                       ((ExpressionView *)right)->baseType);
            if (baseType == NULL)
            {
                reportBinaryOperatorError(
                    line, column, getValueTypeValue(((ExpressionView *)base)->baseType->valueType),
                    getTokenTypeValue(PLUS_TOKEN), getValueTypeValue(((ExpressionView *)right)->baseType->valueType));
                baseType = ((ExpressionView *)base)->baseType;
            }
            base = createBinaryOperator(baseType, base, PLUS_TOKEN, right);
            baseType = computeUnaryOperator(STAR_TOKEN, ((ExpressionView *)base)->baseType);
            if (baseType == NULL)
            {
                reportUnaryOperatorError(line, column, getTokenTypeValue(STAR_TOKEN),
                                         getValueTypeValue(((ExpressionView *)base)->baseType->valueType));
                baseType = ((ExpressionView *)base)->baseType;
            }
            base = createUnaryOperator(baseType, STAR_TOKEN, base);
            break;
        }
        case LEFT_PARENTHESIS: {
            // call()
            nextToken(lexer);
            right = parseExpression(parser, lexer, 0);
            matchToken(lexer, RIGHT_PARENTHESIS);
            // TODO: there should be callable check
            base = createBinaryOperator(((ExpressionView *)base)->baseType, base, CALL_TOKEN, right);
            break;
        }
        default: {
            isDone = 1;
            break;
        }
        }
    }
    return base;
}

Node *parseBinaryExpression(Node *base, Parser *parser, Lexer *lexer, int parentPriority)
{
    peekToken(lexer);
    Node *right = NULL;
    TokenType tokenType = lexer->postToken->tokenType;
    int priority = getBinaryTokenPriority(tokenType);
    int association = getAssociation(tokenType);
    while ((priority != 0) && (association ? priority >= parentPriority : priority > parentPriority))
    {
        nextToken(lexer);
        tokenType = lexer->currToken->tokenType;
        int line = lexer->currToken->line;
        int column = lexer->currToken->column;
        right = parseExpression(parser, lexer, priority);
        BaseType *baseType =
            computeBinaryOperator(((ExpressionView *)base)->baseType, tokenType, ((ExpressionView *)right)->baseType);
        if (baseType == NULL)
        {
            reportBinaryOperatorError(line, column, getValueTypeValue(((ExpressionView *)base)->baseType->valueType),
                                      getTokenTypeValue(tokenType),
                                      getValueTypeValue(((ExpressionView *)right)->baseType->valueType));
            baseType = ((ExpressionView *)base)->baseType;
        }
        base = createBinaryOperator(baseType, base, tokenType, right);
        // peek next token
        peekToken(lexer);
        tokenType = lexer->postToken->tokenType;
        priority = getBinaryTokenPriority(tokenType);
        association = getAssociation(tokenType);
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

void parseStatement(Parser *parser, Lexer *lexer)
{
    peekToken(lexer);
    switch (lexer->postToken->tokenType)
    {
    case INT_TOKEN:
    case CHAR_TOKEN: {
        parseDeclarationStatement(parser, lexer);
        break;
    }
    case IF_TOKEN: {
        parseIfStatement(parser, lexer);
        break;
    }
    case FOR_TOKEN: {
        parseForStatement(parser, lexer);
        break;
    }
    case WHILE_TOKEN: {
        parseWhileStatement(parser, lexer);
        break;
    }
    default: {
        Node *statement = parseExpression(parser, lexer, 0);
        matchToken(lexer, SEMI_COLON_TOKEN);
        appendToList(parser->currScope->list, statement);
        break;
    }
    }
}

BaseType *parsePointerType(BaseType *baseType, Parser *parser, Lexer *lexer)
{
    peekToken(lexer);
    if (lexer->postToken->tokenType == STAR_TOKEN)
    {
        nextToken(lexer);
        baseType = createPointerType(parsePointerType(baseType, parser, lexer));
    }
    return baseType;
}

BaseType *parseArrayType(BaseType *baseType, Parser *parser, Lexer *lexer)
{
    peekToken(lexer);
    if (lexer->postToken->tokenType == LEFT_BRACKET)
    {
        nextToken(lexer);
        int size = 0;
        if (!matchToken(lexer, INT_LITERAL_TOKEN))
        {
            return createArrayType(baseType, size);
        }
        size = atoi(lexer->currToken->value);
        matchToken(lexer, RIGHT_BRACKET);
        baseType = createArrayType(parseArrayType(baseType, parser, lexer), size);
    }
    return baseType;
}

void parseDeclarationStatement(Parser *parser, Lexer *lexer)
{
    nextToken(lexer);
    ValueType valueType = tokenTypeToValueType(lexer->currToken->tokenType);
    peekToken(lexer);
    Node *identifier, *initializer;
    while (1)
    {
        BaseType *baseType = createBaseType(valueType);
        // parse pointer
        baseType = parsePointerType(baseType, parser, lexer);
        // parse identifier
        if (!matchToken(lexer, IDENTIFIER_TOKEN))
        {
            freeBaseType(baseType);
            break;
        }
        identifier = createLiteral(baseType, lexer->currToken->tokenType, lexer->currToken->value);
        int line = lexer->currToken->line;
        int column = lexer->currToken->column;
        // parse array type
        baseType = parseArrayType(baseType, parser, lexer);
        const char *name = ((Literal *)identifier)->value;
        if (tryDeclare(&parser->currScope->table, baseType, name))
        {
            appendToList(parser->currScope->list, createDeclaration(baseType, name));
            initializer = parseBinaryExpression(identifier, parser, lexer, 0);
            if (initializer->nodeType == BINARY_OPERATE_NODE)
                appendToList(parser->currScope->list, initializer);
        }
        else
        {
            reportVariabledefined(line, column, name);
            freeBaseType(baseType);
            freeNode(identifier);
            peekToken(lexer);
            while (lexer->postToken->tokenType != COMMA_TOKEN || lexer->postToken->tokenType != SEMI_COLON_TOKEN)
            {
                // drop initializer part
                nextToken(lexer);
                peekToken(lexer);
            }
        }
        peekToken(lexer);
        if (lexer->postToken->tokenType != COMMA_TOKEN)
            break;
        nextToken(lexer);
    }
    matchToken(lexer, SEMI_COLON_TOKEN);
}

void parseIfStatement(Parser *parser, Lexer *lexer)
{
    List *list = parser->currScope->list;
    matchToken(lexer, IF_TOKEN);
    matchToken(lexer, LEFT_PARENTHESIS);
    int trueEndLabelNumber = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), trueEndLabelNumber));
    matchToken(lexer, RIGHT_PARENTHESIS);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LEFT_BRACE)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
    peekToken(lexer);
    if (lexer->postToken->tokenType == ELSE_TOKEN)
    {
        int falseEndLabelNumber = parser->labelNumber++;
        appendToList(list, createJump(falseEndLabelNumber));
        appendToList(list, createLabel(trueEndLabelNumber));
        parseElseStatement(parser, lexer);
        appendToList(list, createLabel(falseEndLabelNumber));
    }
    else
    {
        appendToList(list, createLabel(trueEndLabelNumber));
    }
}

void parseElseStatement(Parser *parser, Lexer *lexer)
{
    matchToken(lexer, ELSE_TOKEN);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LEFT_BRACE)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
}

void parseForStatement(Parser *parser, Lexer *lexer)
{
    enterScope(parser);

    // parse
    List *list = parser->currScope->list;
    matchToken(lexer, FOR_TOKEN);
    matchToken(lexer, LEFT_PARENTHESIS);
    parseStatement(parser, lexer);
    int ForStartLabelNumber = parser->labelNumber++;
    appendToList(list, createLabel(ForStartLabelNumber));
    int ForEndLabelNumber = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), ForEndLabelNumber));
    matchToken(lexer, SEMI_COLON_TOKEN);
    Node *expression = parseExpression(parser, lexer, 0);
    matchToken(lexer, RIGHT_PARENTHESIS);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LEFT_BRACE)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
    appendToList(list, expression);
    appendToList(list, createJump(ForStartLabelNumber));
    appendToList(list, createLabel(ForEndLabelNumber));

    leaveScope(parser);
}

void parseWhileStatement(Parser *parser, Lexer *lexer)
{
    enterScope(parser);

    List *list = parser->currScope->list;
    matchToken(lexer, WHILE_TOKEN);
    matchToken(lexer, LEFT_PARENTHESIS);
    int whileStartLabelNumber = parser->labelNumber++;
    appendToList(list, createLabel(whileStartLabelNumber));
    int whileEndLabelNumber = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), whileEndLabelNumber));
    matchToken(lexer, RIGHT_PARENTHESIS);
    peekToken(lexer);
    if (lexer->postToken->tokenType == LEFT_BRACE)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
    appendToList(list, createJump(whileStartLabelNumber));
    appendToList(list, createLabel(whileEndLabelNumber));

    leaveScope(parser);
}

/**
 * return this scope for additional option
 */
Node *parseStatements(Parser *parser, Lexer *lexer, int isGlobal)
{
    Node *scope = enterScope(parser);

    if (!isGlobal)
        matchToken(lexer, LEFT_BRACE);
    peekToken(lexer);
    while ((lexer->postToken->tokenType != END_OF_FILE_TOKEN) &&
           ((isGlobal) || (!isGlobal && lexer->postToken->tokenType != RIGHT_BRACE)))
    {
        parseStatement(parser, lexer);
        peekToken(lexer);
    }
    isGlobal ? matchToken(lexer, END_OF_FILE_TOKEN) : matchToken(lexer, RIGHT_BRACE);

    leaveScope(parser);
    return scope;
}

Node *parse(Parser *parser, FILE *file)
{
    Lexer *lexer = createLexer(file);
    Node *root = parseStatements(parser, lexer, 1);
    freeLexer(lexer);
    return root;
}

Node *enterScope(Parser *parser)
{
    // enter new scope
    Node *scope = createScope(parser->currScope);
    parser->currScope = (Scope *)scope;
    return scope;
}

void leaveScope(Parser *parser)
{
    // back to parent scope
    Scope *scope = parser->currScope;
    parser->currScope = scope->parentScope;
    if (parser->currScope != NULL)
        appendToList(parser->currScope->list, scope);
}