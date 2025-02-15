#include "defs.h"
#include "output.h"
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
    case CONTINUE_T: {
        struct Span span = parser->span(parser);
        parser->match(parser, CONTINUE_T);
        if (parser->curr->begin != NULL)
        {
            struct ASTNode *statement = new_jump(parser->curr->begin->value);
            parser->append(parser, statement);
        }
        else
        {
            err_continus(&span);
        }
        parser->match(parser, SEMI_COLON_T);
        break;
    }
    case BREAK_T: {
        struct Span span = parser->span(parser);
        parser->match(parser, BREAK_T);
        if (parser->curr->end != NULL)
        {
            struct ASTNode *statement = new_jump(parser->curr->end->value);
            parser->append(parser, statement);
        }
        else
        {
            err_break(&span);
        }
        parser->match(parser, SEMI_COLON_T);
        break;
    }
    case SEMI_COLON_T: {
        parser->match(parser, SEMI_COLON_T);
        break;
    }
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
    struct ASTNode *decl, *init;
    while (1)
    {
        struct CType *ctype = new_ctype(type, 1);
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
            init = parse_expression(parser, 0);
        }
        else
        {
            init = NULL;
        }
        decl = new_declare(ctype, id, init);
        // check declare
        if (parser->curr->table->try_declare_var(parser->curr->table, ctype, decl->value))
        {
            parser->append(parser, decl);
        }
        else
        {
            err_var_defined(&span, decl->value);
            decl->del(decl);
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
    // labels
    struct ASTNode *forStart = new_label(parser->number++);
    parser->curr->begin = forStart;
    struct ASTNode *forEnd = new_label(parser->number++);
    parser->curr->end = forEnd;
    // for
    parser->match(parser, FOR_T);
    // (
    parser->match(parser, L_PAREN_T);
    // (1;
    parse_statement(parser);
    // for start
    parser->append(parser, forStart);
    // (1;2; jumpfalse for end
    if (parser->token(parser) == SEMI_COLON_T)
        parser->append(parser, new_jump_false(new_literal(new_ctype(INT_TYPE, 0), "1"), forEnd->value));
    else
        parser->append(parser, new_jump_false(parse_expression(parser, 0), forEnd->value));
    parser->match(parser, SEMI_COLON_T);
    // (1;2;3
    struct ASTNode *expression = NULL;
    if (parser->token(parser) != R_PAREN_T)
        expression = parse_expression(parser, 0);
    // (1;2;3)
    parser->match(parser, R_PAREN_T);
    // {}
    parse_statements(parser);
    // 3
    if (expression != NULL)
        parser->append(parser, expression);
    // jump for start
    parser->append(parser, new_jump(forStart->value));
    // for end
    parser->append(parser, forEnd);
    parser->leave(parser);
}

void parse_while(struct Parser *parser)
{
    // label
    struct ASTNode *whileStart = new_label(parser->number++);
    parser->curr->begin = whileStart;
    struct ASTNode *whileEnd = new_label(parser->number++);
    parser->curr->end = whileEnd;
    // while
    parser->match(parser, WHILE_T);
    // (
    parser->match(parser, L_PAREN_T);
    // while start
    parser->append(parser, whileStart);
    // jump false while end
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
