#include "data.h"

/**
 *
 *      Register Allocation
 *
 */
const char *regs[] = {"%eax", "%ebx", "%ecx", "%edx"};
int flags[] = {0, 0, 0, 0};
int curr_reg = 0;
int alloc_reg()
{
    while (flags[curr_reg])
    {
        curr_reg++;
        curr_reg %= 4;
    }
    flags[curr_reg] = 1;
    return curr_reg;
}

const char *reg_name(int reg)
{
    return regs[reg];
}

void free_reg(int reg)
{
    if (reg < 0 || reg >= 4)
        return;
    flags[reg] = 0;
}

void gen_text()
{
    fprintf(cg_out, "\t.text\n");
}

void gen_label(const char *name)
{
    fprintf(cg_out, "%s:\n", name);
}

void gen_movl(const char *left, const char *right)
{
    fprintf(cg_out, "\tmovl\t%s, %s\n", right, left);
}

void gen_addl(const char *left, const char *right)
{
    fprintf(cg_out, "\taddl\t%s, %s\n", right, left);
}

void gen_subl(const char *left, const char *right)
{
    fprintf(cg_out, "\tsubl\t%s, %s\n", right, left);
}

void gen_imull(const char *left)
{
    fprintf(cg_out, "\timull\t%s\n", left);
}

void gen_idivl(const char *left)
{
    fprintf(cg_out, "\tidivl\t%s\n", left);
}

void gen_pushl(const char *left)
{
    fprintf(cg_out, "\tpushl\t%s\n", left);
}

void gen_popl(const char *left)
{
    fprintf(cg_out, "\tpopl\t%s\n", left);
}
