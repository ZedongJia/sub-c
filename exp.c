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
    case INT_LIT_T: {
        expression = cLiteral(createCType(INT_TYPE, 0), lexer->buf);
        match(lexer, INT_LIT_T);
        return expression;
    }
    case STR_LIT_T: {
        CType *ctype = createCType(CHAR_TYPE, 0);
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
                CType *ctype = scope->table->variables[index].ctype;
                expression = cLiteral(cloneCType(ctype), lexer->buf);
                break;
            }
            scope = scope->parent;
        }
        if (!expression)
        {
            reportVariableUndefined(lexer->line, lexer->start, lexer->buf);
            expression = cLiteral(createCType(0, 0), lexer->buf);
        }
        match(lexer, ID_T);
        return expression;
    }
    default: {
        reportUnexpectedToken(lexer->line, lexer->start, tokenName(lexer->token), "expression");
        expression = cLiteral(createCType(INT_TYPE, 0), "0");
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
        CType *ctype = unary_compatible(ukind, operand->ctype);
        if (!ctype)
        {
            reportUnaryInCompatible(line, start, operand->ctype, kindName(ukind));
            ctype = cloneCType(operand->ctype);
        }
        expression = cUnary(ukind, ctype, operand);
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
    CType *ctype;
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
            ctype = binary_compatible(ADD_N, left->ctype, right->ctype);
            if (!ctype)
            {
                reportBinaryInCompatible(line, start, left->ctype, kindName(ADD_N), right->ctype);
                ctype = cloneCType(left->ctype);
            }
            left = cBinary(ADD_N, ctype, left, right);
            ctype = unary_compatible(ADDR_N, left->ctype);
            if (!ctype)
            {
                reportUnaryInCompatible(line, start, left->ctype, kindName(ADDR_N));
                ctype = cloneCType(left->ctype);
            }
            left = cUnary(ADDR_N, ctype, left);
            break;
        }
        case L_PAREN_T: {
            // call()
            match(lexer, L_PAREN_T);
            right = parseExpression(parser, lexer, 0);
            match(lexer, R_PAREN_T);
            // TODO: there should be callable check
            left = cBinary(CALL_N, cloneCType(left->ctype), left, right);
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
    CType *ctype;
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
        ctype = binary_compatible(bkind, left->ctype, right->ctype);
        if (!ctype)
        {
            reportBinaryInCompatible(line, start, left->ctype, kindName(bkind), right->ctype);
            ctype = cloneCType(left->ctype);
        }
        left = cBinary(bkind, ctype, left, right);
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