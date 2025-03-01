#include "data.h"
#include <stdlib.h>

/**
 *
 *      Parse Expression
 *
 */

ast_t *parse_primary()
{
    ast_t *expr = NULL;
    switch (lexer->token)
    {
    case L_PAREN_T: {
        // ( expression )
        lexer->match(lexer, L_PAREN_T);
        expr = parse_expr(0);
        lexer->match(lexer, R_PAREN_T);
        return expr;
    }
    case L_BRC_T: {
        // { expression }
        lexer->match(lexer, L_BRC_T);
        expr = parse_expr(0);
        lexer->match(lexer, R_BRC_T);
        return expr;
    }
    case INT_LIT_T: {
        expr = new_imm(new_ctype(INT_TYPE, 0), lexer->text);
        lexer->match(lexer, INT_LIT_T);
        return expr;
    }
    case STR_LIT_T: {
        ctype_t *ctype = new_ctype(CHAR_TYPE, 0);
        point_to(ctype);
        expr = new_literal(ctype, NULL, lexer->text);
        expr->symbol = const_table->try_declare(const_table, expr->ctype, expr->value, NULL);
        lexer->match(lexer, STR_LIT_T);
        return expr;
    }
    case ID_T: {
        ctype_t *ctype = new_ctype(0, 0);
        const symbol_t *var_symbol = NULL;
        // find symbol
        symbol_table_t *ptr = var_table;
        while (ptr != NULL)
        {
            var_symbol = ptr->look_up(ptr, lexer->text);
            if (var_symbol != NULL)
                break;
            ptr = ptr->prt;
        }
        if (var_symbol != NULL)
        {
            ctype = var_symbol->ctype->clone(var_symbol->ctype);
        }
        else
        {
            span_t span = lexer->span;
            err_var_undefined(&span, lexer->text);
        }
        expr = new_var(ctype, var_symbol, lexer->text);
        lexer->match(lexer, ID_T);
        return expr;
    }
    default: {
        span_t span = lexer->span;
        err_unexpect_token(&span, lexer->token);
        expr = new_imm(new_ctype(INT_TYPE, 0), "0");
        return expr;
    }
    }
}

ast_t *parse_prefix(int prt_prior)
{
    ast_t *expr;
    int ukind = to_usyntax_kind(lexer->token);
    int prior = uprior(ukind);
    if (prior != 0 && prior >= prt_prior)
    {
        // right association
        span_t span = lexer->span;
        lexer->next(lexer);
        ast_t *operand = parse_expr(prior);
        ctype_t *ctype = unary_compatible(ukind, operand->ctype);
        if (!ctype)
        {
            err_incompat_unary(&span, ukind, operand->ctype);
            ctype = operand->ctype->clone(operand->ctype);
        }
        expr = new_unary(ukind, ctype, operand);
        return expr;
    }
    else
    {
        return parse_primary();
    }
}

ast_t *parse_suffix(ast_t *left)
{
    ast_t *right = NULL;
    ctype_t *ctype;
    int isDone = 0;
    while (!isDone)
    {
        switch (lexer->token)
        {
        case L_BRK_T: {
            // []
            span_t span = lexer->span;
            lexer->match(lexer, L_BRK_T);
            right = parse_expr(0);
            lexer->match(lexer, R_BRK_T);
            ctype = binary_compatible(ADD_KIND, left->ctype, right->ctype);
            if (!ctype)
            {
                err_incompat_binary(&span, left->ctype, ADD_KIND, right->ctype);
                ctype = left->ctype->clone(left->ctype);
            }
            left = new_binary(ADD_KIND, ctype, left, right);
            ctype = unary_compatible(ADDR_KIND, left->ctype);
            if (!ctype)
            {
                err_incompat_unary(&span, ADDR_KIND, left->ctype);
                ctype = left->ctype->clone(left->ctype);
            }
            left = new_unary(ADDR_KIND, ctype, left);
            break;
        }
        case L_PAREN_T: {
            // call()
            lexer->match(lexer, L_PAREN_T);
            right = parse_expr(0);
            lexer->match(lexer, R_PAREN_T);
            // TODO: there should be callable check
            left = new_binary(CALL_KIND, left->ctype->clone(left->ctype), left, right);
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

ast_t *parse_binary(ast_t *left, int prt_prior)
{
    ast_t *right = NULL;
    ctype_t *ctype;
    syntax_kind_t bkind;
    int prior;
    while (1)
    {
        bkind = to_bsyntax_kind(lexer->token);
        if (!bkind)
            break;
        prior = bprior(bkind);
        if (assoc(bkind) ? prior < prt_prior : prior <= prt_prior)
            break;
        span_t span = lexer->span;
        lexer->next(lexer);
        right = parse_expr(prior);
        ctype = binary_compatible(bkind, left->ctype, right->ctype);
        if (!ctype)
        {
            err_incompat_binary(&span, left->ctype, bkind, right->ctype);
            ctype = left->ctype->clone(left->ctype);
        }
        left = new_binary(bkind, ctype, left, right);
    }
    return left;
}

ast_t *parse_expr(int prt_prior)
{
    // op expression
    ast_t *left;
    left = parse_prefix(prt_prior);
    left = parse_suffix(left);
    left = parse_binary(left, prt_prior);
    return left;
}

/**
 *
 *      Parse Statement
 *
 */

void parse_enter()
{
    // enter new scope
    scope = new_scope(scope);
    var_table = scope->var_table;
}

void parse_leave()
{
    // back to parent scope
    if (scope->prt == NULL)
    {
        // TODO: throw error
        return;
    }
    ast_t *child = scope;
    scope = child->prt;
    scope->append(scope, child);
    var_table = scope->var_table;
}

void parse_stmt()
{
    switch (lexer->token)
    {
    case INT_T:
    case CHAR_T:
        parse_declare();
        break;
    case IF_T:
        parse_if();
        break;
    case FOR_T:
        parse_for();
        break;
    case WHILE_T:
        parse_while();
        break;
    case CONTINUE_T: {
        span_t span = lexer->span;
        lexer->match(lexer, CONTINUE_T);
        if (scope->begin != NULL)
        {
            ast_t *stmt = new_jump(scope->begin->value);
            scope->append(scope, stmt);
        }
        else
        {
            err_continus(&span);
        }
        lexer->match(lexer, SEMI_COLON_T);
        break;
    }
    case BREAK_T: {
        span_t span = lexer->span;
        lexer->match(lexer, BREAK_T);
        if (scope->end != NULL)
        {
            ast_t *stmt = new_jump(scope->end->value);
            scope->append(scope, stmt);
        }
        else
        {
            err_break(&span);
        }
        lexer->match(lexer, SEMI_COLON_T);
        break;
    }
    case SEMI_COLON_T: {
        lexer->match(lexer, SEMI_COLON_T);
        break;
    }
    default: {
        ast_t *stmt = parse_expr(0);
        lexer->match(lexer, SEMI_COLON_T);
        scope->append(scope, stmt);
        break;
    }
    }
}

void parse_pointer_ctype(ctype_t *ctype)
{
    if (lexer->token == STAR_T)
    {
        lexer->next(lexer);
        parse_pointer_ctype(ctype);
        point_to(ctype);
    }
}

void parse_array_ctype(ctype_t *ctype)
{
    if (lexer->token == L_BRK_T)
    {
        lexer->next(lexer);
        int size = 0;
        if (lexer->token != INT_LIT_T)
        {
            // TODO: error info
        }
        else
        {
            size = atoi(lexer->text);
            lexer->next(lexer);
        }
        lexer->match(lexer, R_BRK_T);
        parse_array_ctype(ctype);
        array_of(ctype, size);
    }
}

void parse_declare()
{
    type_t type = to_type(lexer->token);
    lexer->next(lexer);
    ast_t *decl, *init;
    while (1)
    {
        ctype_t *ctype = new_ctype(type, 1);
        // parse pointer
        parse_pointer_ctype(ctype);
        // parse identifier
        if (lexer->token != ID_T)
        {
            // TODO: error
            ctype->del(ctype);
            break;
        }
        span_t span = lexer->span;
        sprintf(buf, "%s", lexer->text);
        // parse array type
        lexer->next(lexer);
        parse_array_ctype(ctype);
        // parse initializer
        if (lexer->token == EQ_T)
        {
            lexer->next(lexer);
            init = parse_expr(0);
        }
        else
        {
            init = NULL;
        }
        decl = new_declare(ctype, buf, init);
        // check declare
        if (var_table->declare(var_table, ctype, decl->value, NULL))
        {
            scope->append(scope, decl);
        }
        else
        {
            err_var_defined(&span, decl->value);
            decl->del(decl);
        }
        if (lexer->token != COMMA_T)
            break;
        lexer->match(lexer, COMMA_T);
    }
    lexer->match(lexer, SEMI_COLON_T);
}

void parse_if()
{
    // if
    lexer->match(lexer, IF_T);
    // (jumpfalse true end)
    lexer->match(lexer, L_PAREN_T);
    ast_t *end = new_label(label++);
    scope->append(scope, new_jump_false(parse_expr(0), end->value));
    lexer->match(lexer, R_PAREN_T);
    // {}
    parse_statements();
    if (lexer->token == ELSE_T)
        // else
        parse_else(end);
    else
        // true end
        scope->append(scope, end);
}

void parse_else(ast_t *true_end)
{
    ast_t *false_end = new_label(label++);
    // jump false end
    scope->append(scope, new_jump(false_end->value));
    // true end
    scope->append(scope, true_end);
    // else
    lexer->match(lexer, ELSE_T);
    // {}
    parse_statements();
    // false end
    scope->append(scope, false_end);
}

void parse_for()
{
    parse_enter();
    // labels
    ast_t *begin = new_label(label++);
    scope->begin = begin;
    ast_t *end = new_label(label++);
    scope->end = end;
    // for
    lexer->match(lexer, FOR_T);
    // (
    lexer->match(lexer, L_PAREN_T);
    // (1;
    parse_stmt();
    // for start
    scope->append(scope, begin);
    // (1;2; jumpfalse for end
    if (lexer->token == SEMI_COLON_T)
    {
        ctype_t *ctype = new_ctype(INT_TYPE, 0);
        scope->append(scope, new_jump_false(new_imm(ctype, "1"), end->value));
    }
    else
        scope->append(scope, new_jump_false(parse_expr(0), end->value));
    lexer->match(lexer, SEMI_COLON_T);
    // (1;2;3
    ast_t *expression = NULL;
    if (lexer->token != R_PAREN_T)
        expression = parse_expr(0);
    // (1;2;3)
    lexer->match(lexer, R_PAREN_T);
    // {}
    parse_statements();
    // 3
    if (expression != NULL)
        scope->append(scope, expression);
    // jump for start
    scope->append(scope, new_jump(begin->value));
    // for end
    scope->append(scope, end);
    parse_leave();
}

void parse_while()
{
    // label
    ast_t *begin = new_label(label++);
    scope->begin = begin;
    ast_t *end = new_label(label++);
    scope->end = end;
    // while
    lexer->match(lexer, WHILE_T);
    // (
    lexer->match(lexer, L_PAREN_T);
    // while start
    scope->append(scope, begin);
    // jump false while end
    scope->append(scope, new_jump_false(parse_expr(0), end->value));
    // )
    lexer->match(lexer, R_PAREN_T);
    // {}
    parse_statements();
    // jump while start
    scope->append(scope, new_jump(begin->value));
    // while end
    scope->append(scope, end);
}

void parse_statements()
{
    parse_enter();
    if (lexer->token != L_BRC_T)
    {
        // stmt scope
        parse_stmt();
    }
    else
    {
        // stmts scope
        lexer->match(lexer, L_BRC_T);
        while (lexer->token != R_BRC_T)
            parse_stmt();
        lexer->match(lexer, R_BRC_T);
    }
    parse_leave();
}

void parse_program()
{
    lexer->next(lexer);
    while (lexer->token != EOF_T)
        parse_stmt();
    lexer->match(lexer, EOF_T);
}
