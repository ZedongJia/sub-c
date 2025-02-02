#include "stmt.h"
#include "exp.h"
#include <stdlib.h>

void parseStatement(Parser *parser, Lexer *lexer)
{
    switch (lexer->token)
    {
    case INT_T:
    case CHAR_T:
        parseDeclare(parser, lexer);
        break;
    case IF_T:
        parseIf(parser, lexer);
        break;
    case FOR_T:
        parseFor(parser, lexer);
        break;
    case WHILE_T:
        parseWhile(parser, lexer);
        break;
    default: {
        ASTNode *statement = parseExpression(parser, lexer, 0);
        match(lexer, SEMI_COLON_T);
        appendToList(parser->curr->children, statement);
        break;
    }
    }
}

BaseType *__parsePointerType(BaseType *baseType, Parser *parser, Lexer *lexer)
{
    if (lexer->token == STAR_T)
    {
        next(lexer);
        baseType = createPointerType(__parsePointerType(baseType, parser, lexer));
    }
    return baseType;
}

BaseType *__parseArrayType(BaseType *baseType, Parser *parser, Lexer *lexer)
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
        baseType = createArrayType(__parseArrayType(baseType, parser, lexer), size);
    }
    return baseType;
}

void parseDeclare(Parser *parser, Lexer *lexer)
{
    ValueType valueType = tokenTypeToValueType(lexer->token);
    next(lexer);
    ASTNode *declare, *init;
    while (1)
    {
        BaseType *baseType = createBaseType(valueType);
        // parse pointer
        baseType = __parsePointerType(baseType, parser, lexer);
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
        baseType = __parseArrayType(baseType, parser, lexer);
        // declare
        declare->baseType = baseType;
        if (tryDeclare(parser->curr->table, baseType, declare->value))
        {
            appendToList(parser->curr->children, declare);
            if (lexer->token == D_EQ_T)
            {
                init = parseBinary(cLiteral(declare->baseType, declare->value), parser, lexer, 0);
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

void parseIf(Parser *parser, Lexer *lexer)
{
    List *children = parser->curr->children;
    // if
    match(lexer, IF_T);
    // (jumpfalse true end)
    match(lexer, L_PAREN_T);
    ASTNode *trueEnd = cLabel(parser->number++);
    appendToList(children, cJumpFalse(parseExpression(parser, lexer, 0), trueEnd->value));
    match(lexer, R_PAREN_T);
    // {}
    parseStatements(parser, lexer);
    if (lexer->token == ELSE_T)
        // else
        parseElse(parser, lexer, trueEnd);
    else
        // true end
        appendToList(children, trueEnd);
}

void parseElse(Parser *parser, Lexer *lexer, ASTNode *trueEnd)
{
    List *children = parser->curr->children;

    ASTNode *falseEnd = cLabel(parser->number++);
    // jump false end
    appendToList(children, cJump(falseEnd->value));
    // true end
    appendToList(children, trueEnd);
    // else
    match(lexer, ELSE_T);
    // {}
    parseStatements(parser, lexer);
    // false end
    appendToList(children, falseEnd);
}

void parseFor(Parser *parser, Lexer *lexer)
{
    enterScope(parser);
    List *children = parser->curr->children;
    // for
    match(lexer, FOR_T);
    // (
    match(lexer, L_PAREN_T);
    // (1;
    parseStatement(parser, lexer);
    // for start
    ASTNode *forStart = cLabel(parser->number++);
    appendToList(children, forStart);
    // (1;2; jumpfalse for end
    ASTNode *forEnd = cLabel(parser->number++);
    appendToList(children, cJumpFalse(parseExpression(parser, lexer, 0), forEnd->value));
    match(lexer, SEMI_COLON_T);
    // (1;2;3
    ASTNode *expression = parseExpression(parser, lexer, 0);
    // (1;2;3)
    match(lexer, R_PAREN_T);
    // {}
    parseStatements(parser, lexer);
    // 3
    appendToList(children, expression);
    // jump for start
    appendToList(children, cJump(forStart->value));
    // for end
    appendToList(children, forEnd);

    leaveScope(parser);
}

void parseWhile(Parser *parser, Lexer *lexer)
{
    List *children = parser->curr->children;
    // while
    match(lexer, WHILE_T);
    // (
    match(lexer, L_PAREN_T);
    // while start
    ASTNode *whileStart = cLabel(parser->number++);
    appendToList(children, whileStart);
    // jump false while end
    ASTNode *whileEnd = cLabel(parser->number++);
    appendToList(children, cJumpFalse(parseExpression(parser, lexer, 0), whileEnd->value));
    // )
    match(lexer, R_PAREN_T);
    // {}
    parseStatements(parser, lexer);
    // jump while start
    appendToList(children, cJump(whileStart->value));
    // while end
    appendToList(children, whileEnd);
}

/**
 * return this scope for additional option
 */
void parseStatements(Parser *parser, Lexer *lexer)
{
    enterScope(parser);
    if (lexer->token != L_BRC_T)
    {
        // stmt scope
        parseStatement(parser, lexer);
    }
    else
    {
        // stmts scope
        match(lexer, L_BRC_T);
        while (lexer->token != R_BRC_T)
            parseStatement(parser, lexer);
        match(lexer, R_BRC_T);
    }
    leaveScope(parser);
}

ASTNode *parseProgram(Parser *parser, FILE *in)
{
    Lexer lexer;
    initLexer(&lexer, in);
    next(&lexer);
    parser->curr = cScope(NULL);
    while (lexer.token != EOF_T)
        parseStatement(parser, &lexer);
    match(&lexer, EOF_T);
    return parser->curr;
}
