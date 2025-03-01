#include "data.h"
#include "defs.h"
#include <math.h>
#include <stdlib.h>

int gen_expr(ast_t *ast)
{
    switch (ast->kind)
    {
    case ADD_KIND: {
        list_iterator_t *iter = ast->get_iter(ast);
        // left
        int lreg = gen_expr((ast_t *)iter->get(iter));
        iter->next(iter);
        // right
        int rreg = gen_expr((ast_t *)iter->get(iter));
        iter->del(iter);
        // apply add
        gen_addl(reg_name(lreg), reg_name(rreg));
        // free right reg
        free_reg(rreg);
        return lreg;
    }
    case SUB_KIND: {
        list_iterator_t *iter = ast->get_iter(ast);
        // left
        int lreg = gen_expr((ast_t *)iter->get(iter));
        iter->next(iter);
        // right
        int rreg = gen_expr((ast_t *)iter->get(iter));
        iter->del(iter);
        // apply sub
        gen_subl(reg_name(lreg), reg_name(rreg));
        // free right reg
        free_reg(rreg);
        return lreg;
    }
    case MUL_KIND: {
        list_iterator_t *iter = ast->get_iter(ast);
        // left
        int lreg = gen_expr((ast_t *)iter->get(iter));
        iter->next(iter);
        // right
        int rreg = gen_expr((ast_t *)iter->get(iter));
        iter->del(iter);
        if (lreg != 0)
        {
            gen_pushl("%eax");
            gen_movl("%eax", reg_name(lreg));
        }
        // apply mul
        gen_imull(reg_name(rreg));
        if (lreg != 0)
        {
            gen_movl(reg_name(lreg), "%eax");
            gen_popl("%eax");
        }
        // free right reg
        free_reg(rreg);
        return lreg;
    }
    case DIV_KIND: {
        list_iterator_t *iter = ast->get_iter(ast);
        // left
        int lreg = gen_expr((ast_t *)iter->get(iter));
        iter->next(iter);
        // right
        int rreg = gen_expr((ast_t *)iter->get(iter));
        iter->del(iter);
        if (lreg != 0)
        {
            gen_pushl("%eax");
            gen_movl("%eax", reg_name(lreg));
        }
        // apply mul
        gen_idivl(reg_name(rreg));
        if (lreg != 0)
        {
            gen_movl(reg_name(lreg), "%eax");
            gen_popl("%eax");
        }
        // free right reg
        free_reg(rreg);
        return lreg;
    }
    case VAR_KIND: {
        int reg = alloc_reg();
        sprintf(buf, "%d(%%ebp)", -ast->symbol->offset);
        gen_movl(reg_name(reg), buf);
        return reg;
    }
    case IMM_KIND: {
        int reg = alloc_reg();
        gen_movl(reg_name(reg), ast->value);
        return reg;
    }
    case LIT_KIND: {
        int reg = alloc_reg();
        sprintf(buf, "$LC%d", ast->symbol->label);
        gen_movl(reg_name(reg), buf);
        return reg;
    }
    default:
        return -1;
    }
}

void alloc_stack()
{
    // func ? TODO: Fix this
    fprintf(cg_out, "\tpushl\t%%ebp\n");
    gen_movl("%ebp", "%esp");
    fprintf(cg_out, "\tsubl\t$%d, %%esp\n", (int)(ceil((var_table->offset + var_table->call_offset + 8) / 16.0) * 16));
}

void free_stack()
{
    fprintf(cg_out, "\tleave\n");
}

void gen_stmt(ast_t *ast)
{
    switch (ast->kind)
    {
    case DEC_KIND: {
        break;
    }
    case LABEL_KIND: {
        fprintf(cg_out, "%s:\n", ast->value);
        break;
    }
    case JUMP_FALSE_KIND: {
        break;
    }
    case JUMP_KIND: {
        break;
    }
    case SCOPE_KIND: {
        // enter scope
        scope = ast;
        var_table = scope->var_table;

        alloc_stack();
        list_iterator_t *iter = ast->get_iter(ast);
        while (!iter->is_end(iter))
        {
            gen_stmt((ast_t *)iter->get(iter));
            iter->next(iter);
        }
        iter->del(iter);
        free_stack();

        // leave scope
        scope = scope->prt;
        var_table = scope->var_table;
        break;
    }
    default: {
        int reg = gen_expr(ast);
        free_reg(reg);
    }
    }
}

void gen_const()
{
    gen_text();
    fprintf(cg_out, "\t.section .rdata\n");
    int lc = 0;
    list_iterator_t *iter = const_table->symbols->get_iter(const_table->symbols);
    while (!iter->is_end(iter))
    {
        symbol_t *symbol = (symbol_t *)iter->get(iter);
        sprintf(buf, "LC%d", lc);
        gen_label(buf);
        fprintf(cg_out, "\t.ascii \"%s\\0\"\n", symbol->name);
        symbol->label = lc;
        lc++;
        iter->next(iter);
    }
    iter->del(iter);
}

/**
 *     Generate Asembly Code
 */
void gen_program()
{
    gen_const();
    /***
     * Main TODO: temp
     */
    gen_text();
    fprintf(cg_out, "\t.global _main\n");
    gen_label("_main");
    /***
     * Allocate and align
     */
    gen_pushl("%ebp");
    gen_movl("%ebp", "%esp");
    fprintf(cg_out, "\tandl\t$-16, %%esp\n");
    if (var_table->offset != 0)
        fprintf(cg_out, "\tsubl\t$%d, %%esp\n", var_table->offset);
    /***
     * Generator code of stmts
     */
    list_iterator_t *iter = scope->children->get_iter(scope->children);
    while (!iter->is_end(iter))
    {
        gen_stmt((ast_t *)iter->get(iter));
        iter->next(iter);
    }
    iter->del(iter);
    fprintf(cg_out, "\tleave\n");
    fprintf(cg_out, "\tret\n");
}