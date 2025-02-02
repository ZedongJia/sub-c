#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

Parser *createParser()
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->number = 0;
    parser->curr = NULL;
    return parser;
}

void freeParser(Parser *parser)
{
    free(parser);
}

ASTNode *parsePrimaryExpression(Parser *parser, Lexer *lexer)
{
    ASTNode *expression = NULL;
    switch (lexer->token)
    {
    case L_PAREN_T: {
        // ( expression )
        match(lexer, L_PAREN_T);
        expression = parseExpression(parser, lexer, 0);
        match(lexer, R_PAREN_T);
        return expression;
    }
    case L_BRC_T: {
        // { expression }
        match(lexer, L_BRC_T);
        expression = parseExpression(parser, lexer, 0);
        match(lexer, R_BRC_T);
        return expression;
    }
    case TRUE_T:
    case FALSE_T: {
        expression = cLiteral(createBaseType(BOOL_VALUE), lexer->buf);
        next(lexer);
        return expression;
    }
    case INT_LIT_T: {
        expression = cLiteral(createBaseType(INT_VALUE), lexer->buf);
        match(lexer, INT_LIT_T);
        return expression;
    }
    case STR_LIT_T: {
        expression = cLiteral(createPointerType(createBaseType(CHAR_VALUE)), lexer->buf);
        match(lexer, STR_LIT_T);
        return expression;
    }
    case ID_T: {
        ASTNode *curr = parser->curr;
        while (curr != NULL)
        {
            int index = tryLookUp(curr->table, lexer->buf);
            if (index != -1)
            {
                expression = cLiteral(curr->table->variables[index].baseType, lexer->buf);
                break;
            }
            curr = curr->parent;
        }
        if (!expression)
        {
            reportVariableUndefined(lexer->line, lexer->start, lexer->buf);
            expression = cLiteral(createBaseType(UNEXPECTED_VALUE), lexer->buf);
        }
        match(lexer, ID_T);
        return expression;
    }
    default: {
        reportUnexpectedToken(lexer->line, lexer->start, tokenName(lexer->token), "expression");
        expression = cLiteral(createBaseType(INT_LIT_T), "0");
        return expression;
    }
    }
}

ASTNode *parseUnaryExpression(Parser *parser, Lexer *lexer, int parentPriority)
{
    ASTNode *expression;
    int kind = unaryNode(lexer->token);
    int priority = unaryPriority(kind);
    if (priority != 0 && priority >= parentPriority)
    {
        // right association
        int line = lexer->line;
        int start = lexer->start;
        next(lexer);
        ASTNode *operand = parseExpression(parser, lexer, priority);
        BaseType *baseType = computeUnaryOperator(kind, operand->baseType);
        if (baseType == NULL)
        {
            reportUnaryOperatorError(line, start, kindName(kind), getValueTypeValue(operand->baseType->valueType));
            baseType = operand->baseType;
        }
        expression = cUnary(kind, baseType, operand);
        return expression;
    }
    else
    {
        return parsePrimaryExpression(parser, lexer);
    }
}

ASTNode *parseAccessExpression(ASTNode *base, Parser *parser, Lexer *lexer)
{
    ASTNode *right = NULL;
    int isDone = 0;
    while (!isDone)
    {
        switch (lexer->token)
        {
        case L_BRK_T: {
            // []
            int line = lexer->line;
            int start = lexer->start;
            match(lexer, L_BRK_T);
            right = parseExpression(parser, lexer, 0);
            match(lexer, R_BRK_T);
            BaseType *baseType = computeBinaryOperator(base->baseType, ADD_N, right->baseType);
            if (baseType == NULL)
            {
                reportBinaryOperatorError(line, start, getValueTypeValue(base->baseType->valueType), kindName(ADD_N),
                                          getValueTypeValue(right->baseType->valueType));
                baseType = base->baseType;
            }
            base = cBinary(ADD_N, baseType, base, right);
            baseType = computeUnaryOperator(ADDR_N, base->baseType);
            if (baseType == NULL)
            {
                reportUnaryOperatorError(line, start, kindName(ADDR_N), getValueTypeValue(base->baseType->valueType));
                baseType = base->baseType;
            }
            base = cUnary(ADDR_N, baseType, base);
            break;
        }
        case L_PAREN_T: {
            // call()
            match(lexer, L_PAREN_T);
            right = parseExpression(parser, lexer, 0);
            match(lexer, R_PAREN_T);
            // TODO: there should be callable check
            base = cBinary(CALL_N, base->baseType, base, right);
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

ASTNode *parseBinaryExpression(ASTNode *base, Parser *parser, Lexer *lexer, int parentPriority)
{
    ASTNode *right = NULL;
    Kind kind;
    int priority;
    while (1)
    {
        kind = binaryNode(lexer->token);
        if (!kind)
            break;
        priority = binaryPriority(kind);
        if (assoc(kind) ? priority < parentPriority : priority <= parentPriority)
            break;
        int line = lexer->line;
        int start = lexer->start;
        next(lexer);
        right = parseExpression(parser, lexer, priority);
        BaseType *baseType = computeBinaryOperator(base->baseType, kind, right->baseType);
        if (baseType == NULL)
        {
            reportBinaryOperatorError(line, start, getValueTypeValue(base->baseType->valueType), kindName(kind),
                                      getValueTypeValue(right->baseType->valueType));
            baseType = base->baseType;
        }
        base = cBinary(kind, baseType, base, right);
    }
    return base;
}

ASTNode *parseExpression(Parser *parser, Lexer *lexer, int parentPriority)
{
    // op expression
    ASTNode *expression = parseUnaryExpression(parser, lexer, parentPriority);
    expression = parseAccessExpression(expression, parser, lexer);
    expression = parseBinaryExpression(expression, parser, lexer, parentPriority);
    return expression;
}

void parseStatement(Parser *parser, Lexer *lexer)
{
    switch (lexer->token)
    {
    case INT_T:
    case CHAR_T: {
        parseDeclarationStatement(parser, lexer);
        break;
    }
    case IF_T: {
        parseIfStatement(parser, lexer);
        break;
    }
    case FOR_T: {
        parseForStatement(parser, lexer);
        break;
    }
    case WHILE_T: {
        parseWhileStatement(parser, lexer);
        break;
    }
    default: {
        ASTNode *statement = parseExpression(parser, lexer, 0);
        match(lexer, SEMI_COLON_T);
        appendToList(parser->curr->children, statement);
        break;
    }
    }
}

BaseType *parsePointerType(BaseType *baseType, Parser *parser, Lexer *lexer)
{
    if (lexer->token == STAR_T)
    {
        next(lexer);
        baseType = createPointerType(parsePointerType(baseType, parser, lexer));
    }
    return baseType;
}

BaseType *parseArrayType(BaseType *baseType, Parser *parser, Lexer *lexer)
{
    if (lexer->token == L_BRK_T)
    {
        next(lexer);
        int size = 0;
        if (lexer->token != INT_LIT_T)
        {
            // error info
            match(lexer, R_BRK_T);
            return createArrayType(baseType, size);
        }
        size = atoi(lexer->buf);
        next(lexer);
        match(lexer, R_BRK_T);
        baseType = createArrayType(parseArrayType(baseType, parser, lexer), size);
    }
    return baseType;
}

void parseDeclarationStatement(Parser *parser, Lexer *lexer)
{
    ValueType valueType = tokenTypeToValueType(lexer->token);
    next(lexer);
    ASTNode *declare, *init;
    while (1)
    {
        BaseType *baseType = createBaseType(valueType);
        // parse pointer
        baseType = parsePointerType(baseType, parser, lexer);
        // parse identifier
        if (lexer->token != ID_T)
        {
            // TODO: error info
            freeBaseType(baseType);
            break;
        }
        int line = lexer->line;
        int start = lexer->start;
        declare = cDeclare(baseType, lexer->buf);
        // parse array type
        next(lexer);
        baseType = parseArrayType(baseType, parser, lexer);
        // declare
        declare->baseType = baseType;
        if (tryDeclare(parser->curr->table, baseType, declare->value))
        {
            appendToList(parser->curr->children, declare);
            if (lexer->token == D_EQ_T)
            {
                init = parseBinaryExpression(cLiteral(declare->baseType, declare->value), parser, lexer, 0);
                appendToList(parser->curr->children, init);
            }
        }
        else
        {
            reportVariabledefined(line, start, declare->value);
            freeASTNode(declare);
            while (lexer->token != COMMA_T || lexer->token != SEMI_COLON_T)
            {
                // drop init part
                next(lexer);
            }
        }
        if (lexer->token != COMMA_T)
            break;
        match(lexer, COMMA_T);
    }
    match(lexer, SEMI_COLON_T);
}

void parseIfStatement(Parser *parser, Lexer *lexer)
{
    List *children = parser->curr->children;
    match(lexer, IF_T);
    match(lexer, L_PAREN_T);
    ASTNode *trueEnd = cLabel(parser->number++);
    appendToList(children, cJumpFalse(parseExpression(parser, lexer, 0), trueEnd->value));
    match(lexer, R_PAREN_T);
    if (lexer->token == L_BRC_T)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
    if (lexer->token == ELSE_T)
    {
        ASTNode *falseEnd = cLabel(parser->number++);
        appendToList(children, cJump(falseEnd->value));
        appendToList(children, trueEnd);
        parseElseStatement(parser, lexer);
        appendToList(children, falseEnd);
    }
    else
    {
        appendToList(children, trueEnd);
    }
}

void parseElseStatement(Parser *parser, Lexer *lexer)
{
    match(lexer, ELSE_T);
    if (lexer->token == L_BRC_T)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
}

void parseForStatement(Parser *parser, Lexer *lexer)
{
    enterScope(parser);

    // parse
    List *children = parser->curr->children;
    match(lexer, FOR_T);
    match(lexer, L_PAREN_T);
    parseStatement(parser, lexer);
    ASTNode *forStart = cLabel(parser->number++);
    appendToList(children, forStart);
    ASTNode *forEnd = cLabel(parser->number++);
    appendToList(children, cJumpFalse(parseExpression(parser, lexer, 0), forEnd->value));
    match(lexer, SEMI_COLON_T);
    ASTNode *expression = parseExpression(parser, lexer, 0);
    match(lexer, R_PAREN_T);
    if (lexer->token == L_BRC_T)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
    appendToList(children, expression);
    appendToList(children, cJump(forStart->value));
    appendToList(children, forEnd);

    leaveScope(parser);
}

void parseWhileStatement(Parser *parser, Lexer *lexer)
{
    List *children = parser->curr->children;
    match(lexer, WHILE_T);
    match(lexer, L_PAREN_T);
    ASTNode *whileStart = cLabel(parser->number++);
    appendToList(children, whileStart);
    ASTNode *whileEnd = cLabel(parser->number++);
    appendToList(children, cJumpFalse(parseExpression(parser, lexer, 0), whileEnd->value));
    match(lexer, R_PAREN_T);
    if (lexer->token == L_BRC_T)
        parseStatements(parser, lexer, 0);
    else
        parseStatement(parser, lexer);
    appendToList(children, cJump(whileStart->value));
    appendToList(children, whileEnd);
}

/**
 * return this scope for additional option
 */
ASTNode *parseStatements(Parser *parser, Lexer *lexer, int isGlobal)
{
    ASTNode *scope = enterScope(parser);

    if (!isGlobal)
        match(lexer, L_BRC_T);
    while ((lexer->token != EOF_T) && ((isGlobal) || (!isGlobal && lexer->token != R_BRC_T)))
    {
        parseStatement(parser, lexer);
    }
    isGlobal ? match(lexer, EOF_T) : match(lexer, R_BRC_T);

    leaveScope(parser);
    return scope;
}

ASTNode *parse(Parser *parser, FILE *in)
{
    Lexer lexer;
    initLexer(&lexer, in);
    next(&lexer);
    ASTNode *root = parseStatements(parser, &lexer, 1);
    return root;
}

ASTNode *enterScope(Parser *parser)
{
    // enter new scope
    ASTNode *scope = cScope(parser->curr);
    parser->curr = scope;
    return scope;
}

void leaveScope(Parser *parser)
{
    // back to parent scope
    ASTNode *scope = parser->curr;
    parser->curr = scope->parent;
    if (parser->curr != NULL)
        appendToList(parser->curr->children, scope);
}