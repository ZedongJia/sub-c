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
        expression = cLiteral(createCType(BOOL_VALUE), lexer->buf);
        next(lexer);
        return expression;
    }
    case INT_LIT_T: {
        expression = cLiteral(createCType(INT_VALUE), lexer->buf);
        match(lexer, INT_LIT_T);
        return expression;
    }
    case STR_LIT_T: {
        CType *ctype = createCType(CHAR_VALUE);
        point(ctype);
        expression = cLiteral(ctype, lexer->buf);
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
                expression = cLiteral(scope->table->variables[index].ctype, lexer->buf);
                break;
            }
            scope = scope->parent;
        }
        if (!expression)
        {
            reportVariableUndefined(lexer->line, lexer->start, lexer->buf);
            expression = cLiteral(createCType(0), lexer->buf);
        }
        match(lexer, ID_T);
        return expression;
    }
    default: {
        reportUnexpectedToken(lexer->line, lexer->start, tokenName(lexer->token), "expression");
        expression = cLiteral(createCType(INT_VALUE), "0");
        return expression;
    }
    }
}

ASTNode *parsePrefix(Parser *parser, Lexer *lexer, int parentPriority)
{
    ASTNode *expression;
    int ukind = toUKind(lexer->token);
    int priority = unaryPriority(ukind);
    if (priority != 0 && priority >= parentPriority)
    {
        // right association
        int line = lexer->line;
        int start = lexer->start;
        next(lexer);
        ASTNode *operand = parseExpression(parser, lexer, priority);
        // CType *ctype = computeUnaryOperator(kind, operand->ctype);
        // if (ctype == NULL)
        // {
        //     reportUnaryOperatorError(line, start, kindName(kind), typeName(operand->ctype->type));
        //     ctype = operand->ctype;
        // }
        expression = cUnary(ukind, createCType(0), operand);
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
            // CType *ctype = computeBinaryOperator(left->ctype, ADD_N, right->ctype);
            // if (ctype == NULL)
            // {
            //     reportBinaryOperatorError(line, start, typeName(left->ctype->valueType), kindName(ADD_N),
            //                               typeName(right->ctype->valueType));
            //     ctype = left->ctype;
            // }
            left = cBinary(ADD_N, createCType(0), left, right);
            // ctype = computeUnaryOperator(ADDR_N, left->ctype);
            // if (ctype == NULL)
            // {
            //     reportUnaryOperatorError(line, start, kindName(ADDR_N), typeName(left->ctype->valueType));
            //     ctype = left->ctype;
            // }
            left = cUnary(ADDR_N, createCType(0), left);
            break;
        }
        case L_PAREN_T: {
            // call()
            match(lexer, L_PAREN_T);
            right = parseExpression(parser, lexer, 0);
            match(lexer, R_PAREN_T);
            // TODO: there should be callable check
            left = cBinary(CALL_N, left->ctype, left, right);
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
    Kind bkind;
    int priority;
    while (1)
    {
        bkind = toBKind(lexer->token);
        if (!bkind)
            break;
        priority = binaryPriority(bkind);
        if (assoc(bkind) ? priority < parentPriority : priority <= parentPriority)
            break;
        int line = lexer->line;
        int start = lexer->start;
        next(lexer);
        right = parseExpression(parser, lexer, priority);
        // CType *ctype = computeBinaryOperator(left->ctype, bkind, right->ctype);
        // if (ctype == NULL)
        // {
        //     reportBinaryOperatorError(line, start, typeName(left->ctype->type), kindName(bkind),
        //                               typeName(right->ctype->type));
        //     ctype = left->ctype;
        // }
        left = cBinary(bkind, createCType(0), left, right);
    }
    return left;
}

ASTNode *parseExpression(Parser *parser, Lexer *lexer, int parentPriority)
{
    // op expression
    ASTNode *left;
    left = parsePrefix(parser, lexer, parentPriority);
    left = parseSuffix(left, parser, lexer);
    left = parseBinary(left, parser, lexer, parentPriority);
    return left;
}