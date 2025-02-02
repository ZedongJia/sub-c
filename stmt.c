#include "parser.h"
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
        parser->append(parser, statement);
        break;
    }
    }
}

void __parsePointer(CType *ctype, Parser *parser, Lexer *lexer)
{
    if (lexer->token == STAR_T)
    {
        next(lexer);
        __parsePointer(ctype, parser, lexer);
        point(ctype);
    }
}

void __parseArray(CType *ctype, Parser *parser, Lexer *lexer)
{
    if (lexer->token == L_BRK_T)
    {
        next(lexer);
        int size = 0;
        if (lexer->token != INT_LIT_T)
        {
            // TODO: error info
        }
        else
        {
            size = atoi(lexer->buf);
            next(lexer);
        }
        match(lexer, R_BRK_T);
        __parseArray(ctype, parser, lexer);
        array(ctype, size);
    }
}

void parseDeclare(Parser *parser, Lexer *lexer)
{
    Type type = toType(lexer->token);
    next(lexer);
    ASTNode *declare, *initializer;
    while (1)
    {
        CType *ctype = createCType(type, 1);
        // parse pointer
        __parsePointer(ctype, parser, lexer);
        // parse identifier
        if (lexer->token != ID_T)
        {
            // TODO: error
            freeCType(ctype);
            break;
        }
        int line = lexer->line;
        int start = lexer->start;
        char id[256];
        sprintf(id, "%s", lexer->buf);
        // parse array type
        next(lexer);
        __parseArray(ctype, parser, lexer);
        // parse initializer
        if (lexer->token == EQ_T)
        {
            next(lexer);
            initializer = parseExpression(parser, lexer, 0);
        }
        else
        {
            initializer = NULL;
        }
        declare = ASTNode_cDeclare(ctype, id, initializer);
        // check declare
        if (tryDeclare(parser->curr->table, ctype, declare->value))
        {
            parser->append(parser, declare);
        }
        else
        {
            reportVariabledefined(line, start, declare->value);
            ASTNode_del(declare);
        }
        if (lexer->token != COMMA_T)
            break;
        match(lexer, COMMA_T);
    }
    match(lexer, SEMI_COLON_T);
}

void parseIf(Parser *parser, Lexer *lexer)
{
    // if
    match(lexer, IF_T);
    // (jumpfalse true end)
    match(lexer, L_PAREN_T);
    ASTNode *trueEnd = ASTNode_cLabel(parser->number++);
    parser->append(parser, ASTNode_cJumpFalse(parseExpression(parser, lexer, 0), trueEnd->value));
    match(lexer, R_PAREN_T);
    // {}
    parseStatements(parser, lexer);
    if (lexer->token == ELSE_T)
        // else
        parseElse(parser, lexer, trueEnd);
    else
        // true end
        parser->append(parser, trueEnd);
}

void parseElse(Parser *parser, Lexer *lexer, ASTNode *trueEnd)
{
    ASTNode *falseEnd = ASTNode_cLabel(parser->number++);
    // jump false end
    parser->append(parser, ASTNode_cJump(falseEnd->value));
    // true end
    parser->append(parser, trueEnd);
    // else
    match(lexer, ELSE_T);
    // {}
    parseStatements(parser, lexer);
    // false end
    parser->append(parser, falseEnd);
}

void parseFor(Parser *parser, Lexer *lexer)
{
    parser->enter(parser);
    // for
    match(lexer, FOR_T);
    // (
    match(lexer, L_PAREN_T);
    // (1;
    parseStatement(parser, lexer);
    // for start
    ASTNode *forStart = ASTNode_cLabel(parser->number++);
    parser->append(parser, forStart);
    // (1;2; jumpfalse for end
    ASTNode *forEnd = ASTNode_cLabel(parser->number++);
    parser->append(parser, ASTNode_cJumpFalse(parseExpression(parser, lexer, 0), forEnd->value));
    match(lexer, SEMI_COLON_T);
    // (1;2;3
    ASTNode *expression = parseExpression(parser, lexer, 0);
    // (1;2;3)
    match(lexer, R_PAREN_T);
    // {}
    parseStatements(parser, lexer);
    // 3
    parser->append(parser, expression);
    // jump for start
    parser->append(parser, ASTNode_cJump(forStart->value));
    // for end
    parser->append(parser, forEnd);
    parser->leave(parser);
}

void parseWhile(Parser *parser, Lexer *lexer)
{
    // while
    match(lexer, WHILE_T);
    // (
    match(lexer, L_PAREN_T);
    // while start
    ASTNode *whileStart = ASTNode_cLabel(parser->number++);
    parser->append(parser, whileStart);
    // jump false while end
    ASTNode *whileEnd = ASTNode_cLabel(parser->number++);
    parser->append(parser, ASTNode_cJumpFalse(parseExpression(parser, lexer, 0), whileEnd->value));
    // )
    match(lexer, R_PAREN_T);
    // {}
    parseStatements(parser, lexer);
    // jump while start
    parser->append(parser, ASTNode_cJump(whileStart->value));
    // while end
    parser->append(parser, whileEnd);
}

/**
 * return this scope for additional option
 */
void parseStatements(Parser *parser, Lexer *lexer)
{
    parser->enter(parser);
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
    parser->leave(parser);
}

void parseProgram(Parser *parser, FILE *in)
{
    Lexer lexer;
    initLexer(&lexer, in);
    next(&lexer);
    while (lexer.token != EOF_T)
        parseStatement(parser, &lexer);
    match(&lexer, EOF_T);
}
