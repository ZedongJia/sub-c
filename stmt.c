#include "defs.h"
#include "utils.h"
#include <stdlib.h>

void parse_statement(struct Parser *parser)
{
    switch (parser->token(parser))
    {
    case INT_T:
    case CHAR_T:
        parse_declare(parser);
        break;
    case IF_T:
        parse_if(parser);
        break;
    case FOR_T:
        parse_for(parser);
        break;
    case WHILE_T:
        parse_while(parser);
        break;
    default: {
        struct ASTNode *statement = parse_expression(parser, 0);
        parser->match(parser, SEMI_COLON_T);
        parser->append(parser, statement);
        break;
    }
    }
}

void __parsePointer(struct CType *ctype, struct Parser *parser)
{
    if (parser->token(parser) == STAR_T)
    {
        parser->next(parser);
        __parsePointer(ctype, parser);
        point(ctype);
    }
}

void __parseArray(struct CType *ctype, struct Parser *parser)
{
    if (parser->token(parser) == L_BRK_T)
    {
        parser->next(parser);
        int size = 0;
        if (parser->token(parser) != INT_LIT_T)
        {
            // TODO: error info
        }
        else
        {
            size = atoi(parser->value(parser));
            parser->next(parser);
        }
        parser->match(parser, R_BRK_T);
        __parseArray(ctype, parser);
        array(ctype, size);
    }
}

void parse_declare(struct Parser *parser)
{
    Type type = to_type(parser->token(parser));
    parser->next(parser);
    struct ASTNode *declare, *initializer;
    while (1)
    {
        struct CType *ctype = new_CType(type, 1);
        // parse pointer
        __parsePointer(ctype, parser);
        // parse identifier
        if (parser->token(parser) != ID_T)
        {
            // TODO: error
            ctype->del(ctype);
            break;
        }
        struct Span span = parser->span(parser);
        char id[256];
        sprintf(id, "%s", parser->value(parser));
        // parse array type
        parser->next(parser);
        __parseArray(ctype, parser);
        // parse initializer
        if (parser->token(parser) == EQ_T)
        {
            parser->next(parser);
            initializer = parse_expression(parser, 0);
        }
        else
        {
            initializer = NULL;
        }
        declare = new_declare(ctype, id, initializer);
        // check declare
        if (try_declare(parser->curr->table, ctype, declare->value))
        {
            parser->append(parser, declare);
        }
        else
        {
            __err_var_defined(&span, declare->value);
            declare->del(declare);
        }
        if (parser->token(parser) != COMMA_T)
            break;
        parser->match(parser, COMMA_T);
    }
    parser->match(parser, SEMI_COLON_T);
}

void parse_if(struct Parser *parser)
{
    // if
    parser->match(parser, IF_T);
    // (jumpfalse true end)
    parser->match(parser, L_PAREN_T);
    struct ASTNode *trueEnd = new_label(parser->number++);
    parser->append(parser, new_jump_false(parse_expression(parser, 0), trueEnd->value));
    parser->match(parser, R_PAREN_T);
    // {}
    parse_statements(parser);
    if (parser->token(parser) == ELSE_T)
        // else
        parse_else(parser, trueEnd);
    else
        // true end
        parser->append(parser, trueEnd);
}

void parse_else(struct Parser *parser, struct ASTNode *trueEnd)
{
    struct ASTNode *falseEnd = new_label(parser->number++);
    // jump false end
    parser->append(parser, new_jump(falseEnd->value));
    // true end
    parser->append(parser, trueEnd);
    // else
    parser->match(parser, ELSE_T);
    // {}
    parse_statements(parser);
    // false end
    parser->append(parser, falseEnd);
}

void parse_for(struct Parser *parser)
{
    parser->enter(parser);
    // for
    parser->match(parser, FOR_T);
    // (
    parser->match(parser, L_PAREN_T);
    // (1;
    parse_statement(parser);
    // for start
    struct ASTNode *forStart = new_label(parser->number++);
    parser->append(parser, forStart);
    // (1;2; jumpfalse for end
    struct ASTNode *forEnd = new_label(parser->number++);
    parser->append(parser, new_jump_false(parse_expression(parser, 0), forEnd->value));
    parser->match(parser, SEMI_COLON_T);
    // (1;2;3
    struct ASTNode *expression = parse_expression(parser, 0);
    // (1;2;3)
    parser->match(parser, R_PAREN_T);
    // {}
    parse_statements(parser);
    // 3
    parser->append(parser, expression);
    // jump for start
    parser->append(parser, new_jump(forStart->value));
    // for end
    parser->append(parser, forEnd);
    parser->leave(parser);
}

void parse_while(struct Parser *parser)
{
    // while
    parser->match(parser, WHILE_T);
    // (
    parser->match(parser, L_PAREN_T);
    // while start
    struct ASTNode *whileStart = new_label(parser->number++);
    parser->append(parser, whileStart);
    // jump false while end
    struct ASTNode *whileEnd = new_label(parser->number++);
    parser->append(parser, new_jump_false(parse_expression(parser, 0), whileEnd->value));
    // )
    parser->match(parser, R_PAREN_T);
    // {}
    parse_statements(parser);
    // jump while start
    parser->append(parser, new_jump(whileStart->value));
    // while end
    parser->append(parser, whileEnd);
}

/**
 * return this scope for additional option
 */
void parse_statements(struct Parser *parser)
{
    parser->enter(parser);
    if (parser->token(parser) != L_BRC_T)
    {
        // stmt scope
        parse_statement(parser);
    }
    else
    {
        // stmts scope
        parser->match(parser, L_BRC_T);
        while (parser->token(parser) != R_BRC_T)
            parse_statement(parser);
        parser->match(parser, R_BRC_T);
    }
    parser->leave(parser);
}

void parse_program(struct Parser *parser)
{
    parser->next(parser);
    while (parser->token(parser) != EOF_T)
        parse_statement(parser);
    parser->match(parser, EOF_T);
}
