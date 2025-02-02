#include "exp.h"

ASTNode *parsePrimary(Parser *parser, Lexer *lexer)
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
        ASTNode *scope = parser->curr;
        while (scope != NULL)
        {
            int index = tryLookUp(scope->table, lexer->buf);
            if (index != -1)
            {
                expression = cLiteral(scope->table->variables[index].baseType, lexer->buf);
                break;
            }
            scope = scope->parent;
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

ASTNode *parsePrefix(Parser *parser, Lexer *lexer, int parentPriority)
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
        return parsePrimary(parser, lexer);
    }
}

ASTNode *parseSuffix(ASTNode *left, Parser *parser, Lexer *lexer)
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
            BaseType *baseType = computeBinaryOperator(left->baseType, ADD_N, right->baseType);
            if (baseType == NULL)
            {
                reportBinaryOperatorError(line, start, getValueTypeValue(left->baseType->valueType), kindName(ADD_N),
                                          getValueTypeValue(right->baseType->valueType));
                baseType = left->baseType;
            }
            left = cBinary(ADD_N, baseType, left, right);
            baseType = computeUnaryOperator(ADDR_N, left->baseType);
            if (baseType == NULL)
            {
                reportUnaryOperatorError(line, start, kindName(ADDR_N), getValueTypeValue(left->baseType->valueType));
                baseType = left->baseType;
            }
            left = cUnary(ADDR_N, baseType, left);
            break;
        }
        case L_PAREN_T: {
            // call()
            match(lexer, L_PAREN_T);
            right = parseExpression(parser, lexer, 0);
            match(lexer, R_PAREN_T);
            // TODO: there should be callable check
            left = cBinary(CALL_N, left->baseType, left, right);
            break;
        }
        default: {
            isDone = 1;
            break;
        }
        }
    }
    return left;
}

ASTNode *parseBinary(ASTNode *left, Parser *parser, Lexer *lexer, int parentPriority)
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
        BaseType *baseType = computeBinaryOperator(left->baseType, kind, right->baseType);
        if (baseType == NULL)
        {
            reportBinaryOperatorError(line, start, getValueTypeValue(left->baseType->valueType), kindName(kind),
                                      getValueTypeValue(right->baseType->valueType));
            baseType = left->baseType;
        }
        left = cBinary(kind, baseType, left, right);
    }
    return left;
}

ASTNode *parseExpression(Parser *parser, Lexer *lexer, int parentPriority)
{
    // op expression
    ASTNode *left = parsePrefix(parser, lexer, parentPriority);
    left = parseSuffix(left, parser, lexer);
    left = parseBinary(left, parser, lexer, parentPriority);
    return left;
}