#include "defs.h"
#include "utils.h"

struct ASTNode *parse_primary(struct Parser *parser)
{
    struct ASTNode *expression = NULL;
    switch (parser->token(parser))
    {
    case L_PAREN_T: {
        // ( expression )
        parser->match(parser, L_PAREN_T);
        expression = parse_expression(parser, 0);
        parser->match(parser, R_PAREN_T);
        return expression;
    }
    case L_BRC_T: {
        // { expression }
        parser->match(parser, L_BRC_T);
        expression = parse_expression(parser, 0);
        parser->match(parser, R_BRC_T);
        return expression;
    }
    case INT_LIT_T: {
        expression = ASTNode_cliteral(create_CType(INT_TYPE, 0), parser->value(parser));
        parser->match(parser, INT_LIT_T);
        return expression;
    }
    case STR_LIT_T: {
        struct CType *ctype = create_CType(CHAR_TYPE, 0);
        point(ctype);
        expression = ASTNode_cliteral(ctype, parser->value(parser));
        parser->match(parser, STR_LIT_T);
        return expression;
    }
    case ID_T: {
        struct ASTNode *scope = parser->curr;
        while (scope != NULL)
        {
            int index = try_look_up(scope->table, parser->value(parser));
            if (index != -1)
            {
                struct CType *ctype = scope->table->variables[index].ctype;
                expression = ASTNode_cliteral(ctype->clone(ctype), parser->value(parser));
                break;
            }
            scope = scope->prt;
        }
        if (!expression)
        {
            struct Span span = parser->span(parser);
            __err_var_undefined(&span, parser->value(parser));
            expression = ASTNode_cliteral(create_CType(0, 0), parser->value(parser));
        }
        parser->match(parser, ID_T);
        return expression;
    }
    default: {
        struct Span span = parser->span(parser);
        __err_unexpect_token(&span, parser->token(parser));
        expression = ASTNode_cliteral(create_CType(INT_TYPE, 0), "0");
        return expression;
    }
    }
}

struct ASTNode *parse_prefix(struct Parser *parser, int prt_prior)
{
    struct ASTNode *expression;
    int ukind = to_ukind(parser->token(parser));
    int prior = uprior(ukind);
    if (prior != 0 && prior >= prt_prior)
    {
        // right association
        struct Span span = parser->span(parser);
        parser->next(parser);
        struct ASTNode *operand = parse_expression(parser, prior);
        struct CType *ctype = unary_compatible(ukind, operand->ctype);
        if (!ctype)
        {
            __err_incompat_unary(&span, ukind, operand->ctype);
            ctype = operand->ctype->clone(operand->ctype);
        }
        expression = ASTNode_cunary(ukind, ctype, operand);
        return expression;
    }
    else
    {
        return parse_primary(parser);
    }
}

struct ASTNode *parse_suffix(struct ASTNode *left, struct Parser *parser)
{
    struct ASTNode *right = NULL;
    struct CType *ctype;
    int isDone = 0;
    while (!isDone)
    {
        switch (parser->token(parser))
        {
        case L_BRK_T: {
            // []
            struct Span span = parser->span(parser);
            parser->match(parser, L_BRK_T);
            right = parse_expression(parser, 0);
            parser->match(parser, R_BRK_T);
            ctype = binary_compatible(ADD_N, left->ctype, right->ctype);
            if (!ctype)
            {
                __err_incompat_binary(&span, left->ctype, ADD_N, right->ctype);
                ctype = left->ctype->clone(left->ctype);
            }
            left = ASTNode_cbinary(ADD_N, ctype, left, right);
            ctype = unary_compatible(ADDR_N, left->ctype);
            if (!ctype)
            {
                __err_incompat_unary(&span, ADDR_N, left->ctype);
                ctype = left->ctype->clone(left->ctype);
            }
            left = ASTNode_cunary(ADDR_N, ctype, left);
            break;
        }
        case L_PAREN_T: {
            // call()
            parser->match(parser, L_PAREN_T);
            right = parse_expression(parser, 0);
            parser->match(parser, R_PAREN_T);
            // TODO: there should be callable check
            left = ASTNode_cbinary(CALL_N, left->ctype->clone(left->ctype), left, right);
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

struct ASTNode *parse_binary(struct ASTNode *left, struct Parser *parser, int prt_prior)
{
    struct ASTNode *right = NULL;
    struct CType *ctype;
    Kind bkind;
    int prior;
    while (1)
    {
        bkind = to_bkind(parser->token(parser));
        if (!bkind)
            break;
        prior = bprior(bkind);
        if (assoc(bkind) ? prior < prt_prior : prior <= prt_prior)
            break;
        struct Span span = parser->span(parser);
        parser->next(parser);
        right = parse_expression(parser, prior);
        ctype = binary_compatible(bkind, left->ctype, right->ctype);
        if (!ctype)
        {
            __err_incompat_binary(&span, left->ctype, bkind, right->ctype);
            ctype = left->ctype->clone(left->ctype);
        }
        left = ASTNode_cbinary(bkind, ctype, left, right);
    }
    return left;
}

struct ASTNode *parse_expression(struct Parser *parser, int prt_prior)
{
    // op expression
    struct ASTNode *left;
    left = parse_prefix(parser, prt_prior);
    left = parse_suffix(left, parser);
    left = parse_binary(left, parser, prt_prior);
    return left;
}