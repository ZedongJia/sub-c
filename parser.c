#include "parser.h"
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
    Node *expression = NULL;
    switch (lexer->token)
    {
    case LEFT_PARENTHESIS: {
        // ( expression )
        next(lexer);
        expression = parseExpression(parser, lexer, 0);
        match(lexer, RIGHT_PARENTHESIS);
        return expression;
    }
    case LEFT_BRACE: {
        // { expression }
        next(lexer);
        expression = parseExpression(parser, lexer, 0);
        match(lexer, RIGHT_BRACE);
        return expression;
    }
    case TRUE_TOKEN:
    case FALSE_TOKEN: {
        expression = createLiteral(createBaseType(BOOL_VALUE), lexer->token, lexer->buf);
        next(lexer);
        return expression;
    }
    case INT_LITERAL_TOKEN: {
        expression = createLiteral(createBaseType(INT_VALUE), lexer->token, lexer->buf);
        next(lexer);
        return expression;
    }
    case STRING_LITERAL_TOKEN: {
        expression = createLiteral(createPointerType(createBaseType(CHAR_VALUE)), lexer->token, lexer->buf);
        next(lexer);
        return expression;
    }
    case IDENTIFIER_TOKEN: {
        Scope *curr = parser->currScope;
        while (curr != NULL)
        {
            int index = tryLookUp(&curr->table, lexer->buf);
            if (index != -1)
            {
                expression = createLiteral(curr->table.variables[index].baseType, lexer->token, lexer->buf);
                break;
            }
            curr = curr->parentScope;
        }
        if (!expression)
        {
            reportVariableUndefined(lexer->line, lexer->start, lexer->buf);
            expression = createLiteral(createBaseType(UNEXPECTED_VALUE), lexer->token, lexer->buf);
        }
        next(lexer);
        return expression;
    }
    default: {
        reportUnexpectedToken(lexer->line, lexer->start, getTokenTypeValue(lexer->token), "expression");
        expression = createNode(UNEXPECTED_NODE);
        next(lexer);
        return expression;
    }
    }
}

Node *parseUnaryExpression(Parser *parser, Lexer *lexer, int parentPriority)
{
    Node *expression;
    int priority = getUnaryTokenPriority(lexer->token);
    if (priority != 0 && priority >= parentPriority)
    {
        // right association
        int line = lexer->line;
        int start = lexer->start;
        TokenType token = lexer->token;
        next(lexer);
        Node *operand = parseExpression(parser, lexer, priority);
        BaseType *baseType = computeUnaryOperator(token, ((ExpressionView *)operand)->baseType);
        if (baseType == NULL)
        {
            reportUnaryOperatorError(line, start, getTokenTypeValue(token),
                                     getValueTypeValue(((ExpressionView *)operand)->baseType->valueType));
            baseType = ((ExpressionView *)operand)->baseType;
        }
        expression = createUnaryOperator(baseType, token, operand);
        return expression;
    }
    else
    {
        return parsePrimaryExpression(parser, lexer);
    }
}

Node *parseAccessExpression(Node *base, Parser *parser, Lexer *lexer)
{
    Node *right = NULL;
    TokenType token;
    int isDone = 0;
    while (!isDone)
    {
        token = lexer->token;
        switch (token)
        {
        case LEFT_BRACKET: {
            // []
            int line = lexer->line;
            int start = lexer->start;
            next(lexer);
            right = parseExpression(parser, lexer, 0);
            match(lexer, RIGHT_BRACKET);
            BaseType *baseType = computeBinaryOperator(((ExpressionView *)base)->baseType, PLUS_TOKEN,
                                                       ((ExpressionView *)right)->baseType);
            if (baseType == NULL)
            {
                reportBinaryOperatorError(line, start, getValueTypeValue(((ExpressionView *)base)->baseType->valueType),
                                          getTokenTypeValue(PLUS_TOKEN),
                                          getValueTypeValue(((ExpressionView *)right)->baseType->valueType));
                baseType = ((ExpressionView *)base)->baseType;
            }
            base = createBinaryOperator(baseType, base, PLUS_TOKEN, right);
            baseType = computeUnaryOperator(STAR_TOKEN, ((ExpressionView *)base)->baseType);
            if (baseType == NULL)
            {
                reportUnaryOperatorError(line, start, getTokenTypeValue(STAR_TOKEN),
                                         getValueTypeValue(((ExpressionView *)base)->baseType->valueType));
                baseType = ((ExpressionView *)base)->baseType;
            }
            base = createUnaryOperator(baseType, STAR_TOKEN, base);
            break;
        }
        case LEFT_PARENTHESIS: {
            // call()
            next(lexer);
            right = parseExpression(parser, lexer, 0);
            match(lexer, RIGHT_PARENTHESIS);
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
    Node *right = NULL;
    TokenType token = lexer->token;
    int priority = getBinaryTokenPriority(token);
    int association = getAssociation(token);
    while ((priority != 0) && (association ? priority >= parentPriority : priority > parentPriority))
    {
        int line = lexer->line;
        int start = lexer->start;
        token = lexer->token;
        next(lexer);
        right = parseExpression(parser, lexer, priority);
        BaseType *baseType =
            computeBinaryOperator(((ExpressionView *)base)->baseType, token, ((ExpressionView *)right)->baseType);
        if (baseType == NULL)
        {
            reportBinaryOperatorError(line, start, getValueTypeValue(((ExpressionView *)base)->baseType->valueType),
                                      getTokenTypeValue(token),
                                      getValueTypeValue(((ExpressionView *)right)->baseType->valueType));
            baseType = ((ExpressionView *)base)->baseType;
        }
        base = createBinaryOperator(baseType, base, token, right);
        token = lexer->token;
        priority = getBinaryTokenPriority(token);
        association = getAssociation(token);
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
    switch (lexer->token)
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
        match(lexer, SEMI_COLON_TOKEN);
        appendToList(parser->currScope->list, statement);
        break;
    }
    }
}

BaseType *parsePointerType(BaseType *baseType, Parser *parser, Lexer *lexer)
{
    if (lexer->token == STAR_TOKEN)
    {
        next(lexer);
        baseType = createPointerType(parsePointerType(baseType, parser, lexer));
    }
    return baseType;
}

BaseType *parseArrayType(BaseType *baseType, Parser *parser, Lexer *lexer)
{
    if (lexer->token == LEFT_BRACKET)
    {
        next(lexer);
        int size = 0;
        if (lexer->token != INT_LITERAL_TOKEN)
        {
            // error info
            match(lexer, RIGHT_BRACKET);
            return createArrayType(baseType, size);
        }
        size = atoi(lexer->buf);
        next(lexer);
        match(lexer, RIGHT_BRACKET);
        baseType = createArrayType(parseArrayType(baseType, parser, lexer), size);
    }
    return baseType;
}

void parseDeclarationStatement(Parser *parser, Lexer *lexer)
{
    ValueType valueType = tokenTypeToValueType(lexer->token);
    next(lexer);
    Node *identifier, *initializer;
    while (1)
    {
        BaseType *baseType = createBaseType(valueType);
        // parse pointer
        baseType = parsePointerType(baseType, parser, lexer);
        // parse identifier
        if (lexer->token != IDENTIFIER_TOKEN)
        {
            // TODO: error info
            freeBaseType(baseType);
            break;
        }
        identifier = createLiteral(baseType, lexer->token, lexer->buf);
        int line = lexer->line;
        int start = lexer->start;
        // parse array type
        next(lexer);
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
            reportVariabledefined(line, start, name);
            freeBaseType(baseType);
            freeNode(identifier);
            while (lexer->token != COMMA_TOKEN || lexer->token != SEMI_COLON_TOKEN)
            {
                // drop initializer part
                next(lexer);
            }
        }
        if (lexer->token != COMMA_TOKEN)
            break;
        match(lexer, COMMA_TOKEN);
    }
    match(lexer, SEMI_COLON_TOKEN);
}

void parseIfStatement(Parser *parser, Lexer *lexer)
{
    List *list = parser->currScope->list;
    match(lexer, IF_TOKEN);
    match(lexer, LEFT_PARENTHESIS);
    int trueEnd = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), trueEnd));
    match(lexer, RIGHT_PARENTHESIS);
    if (lexer->token == LEFT_BRACE)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
    if (lexer->token == ELSE_TOKEN)
    {
        int falseEnd = parser->labelNumber++;
        appendToList(list, createJump(falseEnd));
        appendToList(list, createLabel(trueEnd));
        parseElseStatement(parser, lexer);
        appendToList(list, createLabel(falseEnd));
    }
    else
    {
        appendToList(list, createLabel(trueEnd));
    }
}

void parseElseStatement(Parser *parser, Lexer *lexer)
{
    match(lexer, ELSE_TOKEN);
    if (lexer->token == LEFT_BRACE)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
}

void parseForStatement(Parser *parser, Lexer *lexer)
{
    enterScope(parser);

    // parse
    List *list = parser->currScope->list;
    match(lexer, FOR_TOKEN);
    match(lexer, LEFT_PARENTHESIS);
    parseStatement(parser, lexer);
    int forStart = parser->labelNumber++;
    appendToList(list, createLabel(forStart));
    int forEnd = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), forEnd));
    match(lexer, SEMI_COLON_TOKEN);
    Node *expression = parseExpression(parser, lexer, 0);
    match(lexer, RIGHT_PARENTHESIS);
    if (lexer->token == LEFT_BRACE)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
    appendToList(list, expression);
    appendToList(list, createJump(forStart));
    appendToList(list, createLabel(forEnd));

    leaveScope(parser);
}

void parseWhileStatement(Parser *parser, Lexer *lexer)
{
    enterScope(parser);

    List *list = parser->currScope->list;
    match(lexer, WHILE_TOKEN);
    match(lexer, LEFT_PARENTHESIS);
    int whileStart = parser->labelNumber++;
    appendToList(list, createLabel(whileStart));
    int whileEnd = parser->labelNumber++;
    appendToList(list, createJumpIfFalse(parseExpression(parser, lexer, 0), whileEnd));
    match(lexer, RIGHT_PARENTHESIS);
    if (lexer->token == LEFT_BRACE)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
    appendToList(list, createJump(whileStart));
    appendToList(list, createLabel(whileEnd));

    leaveScope(parser);
}

/**
 * return this scope for additional option
 */
Node *parseStatements(Parser *parser, Lexer *lexer, int isGlobal)
{
    Node *scope = enterScope(parser);

    if (!isGlobal)
        match(lexer, LEFT_BRACE);
    while ((lexer->token != END_OF_FILE_TOKEN) && ((isGlobal) || (!isGlobal && lexer->token != RIGHT_BRACE)))
    {
        parseStatement(parser, lexer);
    }
    isGlobal ? match(lexer, END_OF_FILE_TOKEN) : match(lexer, RIGHT_BRACE);

    leaveScope(parser);
    return scope;
}

Node *parse(Parser *parser, FILE *in)
{
    Lexer lexer;
    initLexer(&lexer, in);
    next(&lexer);
    Node *root = parseStatements(parser, &lexer, 1);
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