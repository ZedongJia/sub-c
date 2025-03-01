#include "defs.h"

const char *token_name(token_t token)
{
    switch (token)
    {
    case INT_LIT_T:
        return "Int Literal";
    case STR_LIT_T:
        return "String Literal";
    // token-keywords
    case INT_T:
        return "Int";
    case CHAR_T:
        return "Char";
    // keywords
    case ID_T:
        return "Identifier";
    case IF_T:
        return "If";
    case ELSE_T:
        return "Else";
    case FOR_T:
        return "For";
    case WHILE_T:
        return "While";
    case CONTINUE_T:
        return "Continue";
    case BREAK_T:
        return "Break";
    // scope
    case L_PAREN_T:
        return "(";
    case R_PAREN_T:
        return ")";
    case L_BRK_T:
        return "[";
    case R_BRK_T:
        return "]";
    case L_BRC_T:
        return "{";
    case R_BRC_T:
        return "}";
    // operator
    case PLUS_T:
        return "+";
    case MIN_T:
        return "-";
    case STAR_T:
        return "*";
    case SLASH_T:
        return "/";
    case EQ_T:
        return "=";
    case GT_T:
        return ">";
    case GE_T:
        return ">=";
    case LT_T:
        return "<";
    case LE_T:
        return "<=";
    case D_EQ_T:
        return "==";
    case NE_T:
        return "!=";
    case D_AND_T:
        return "&&";
    case D_OR_T:
        return "||";
    case AND_T:
        return "&";
    case OR_T:
        return "|";
    case NOT_T:
        return "!";
    // separator
    case COMMA_T:
        return ",";
    case SEMI_COLON_T:
        return ";";
    // other
    case EOF_T:
        return "EOF";
    default:
        return "?";
    }
}

const char *syntax_kind_name(syntax_kind_t kind)
{
    switch (kind)
    {
    case IMM_KIND:
        return "Immediate";
    case LIT_KIND:
        return "Literal";
    case VAR_KIND:
        return "Variable";
    // unary operator
    case ADDR_KIND:
        return "*";
    case ADDR_OF_KIND:
        return "&";
    case P_ADD_KIND:
        return "+";
    case P_SUB_KIND:
        return "-";
    case NOT_KIND:
        return "!";
    // binary operator
    case ASSIGN_KIND:
        return "=";
    case ADD_KIND:
        return "+";
    case SUB_KIND:
        return "-";
    case MUL_KIND:
        return "*";
    case DIV_KIND:
        return "/";
    case GT_KIND:
        return ">";
    case GE_KIND:
        return ">=";
    case LT_KIND:
        return "<";
    case LE_KIND:
        return "<=";
    case EQ_KIND:
        return "==";
    case NE_KIND:
        return "!=";
    case AND_KIND:
        return "&&";
    case OR_KIND:
        return "||";
    case L_AND_KIND:
        return "&";
    case L_OR_KIND:
        return "|";
    // multi operator
    case CALL_KIND:
        return "call(...)";
    case COMMA_KIND:
        return "...,...";
    case DEC_KIND:
        return "Declaration";
    case LABEL_KIND:
        return "Label";
    case JUMP_FALSE_KIND:
        return "Jump If False";
    case JUMP_KIND:
        return "Jump";
    case SCOPE_KIND:
        return "Scope";
    default:
        return "?";
    }
}

int uprior(syntax_kind_t kind)
{
    switch (kind)
    {
    case P_ADD_KIND:
    case P_SUB_KIND:
    case NOT_KIND:
    case ADDR_KIND:
    case ADDR_OF_KIND:
        return 9;
    default:
        return 0;
    }
}

int bprior(syntax_kind_t kind)
{
    switch (kind)
    {
    case MUL_KIND:
    case DIV_KIND:
        return 8;
    case ADD_KIND:
    case SUB_KIND:
        return 7;
    case GT_KIND:
    case GE_KIND:
    case LT_KIND:
    case LE_KIND:
        return 6;
    case EQ_KIND:
    case NE_KIND:
        return 5;
    case AND_KIND:
    case OR_KIND:
        return 4;
    case L_AND_KIND:
    case L_OR_KIND:
        return 3;
    case ASSIGN_KIND:
        return 2;
    case COMMA_KIND:
        return 1;
    default:
        return 0;
    }
}

int assoc(syntax_kind_t kind)
{
    switch (kind)
    {
    case ASSIGN_KIND:
        return 1;
    default:
        return 0;
    }
}

syntax_kind_t to_usyntax_kind(token_t token)
{
    switch (token)
    {
    case PLUS_T:
        return P_ADD_KIND;
    case MIN_T:
        return P_SUB_KIND;
    case NOT_T:
        return NOT_KIND;
    case AND_T:
        return ADDR_OF_KIND;
    case STAR_T:
        return ADDR_KIND;
    default:
        return 0;
    }
}

syntax_kind_t to_bsyntax_kind(token_t token)
{
    switch (token)
    {
    case STAR_T:
        return MUL_KIND;
    case SLASH_T:
        return DIV_KIND;
    case PLUS_T:
        return ADD_KIND;
    case MIN_T:
        return SUB_KIND;
    case GT_T:
        return GT_KIND;
    case GE_T:
        return GE_KIND;
    case LT_T:
        return LT_KIND;
    case LE_T:
        return LE_KIND;
    case D_EQ_T:
        return EQ_KIND;
    case NE_T:
        return NE_KIND;
    case D_AND_T:
        return AND_KIND;
    case D_OR_T:
        return OR_KIND;
    case AND_T:
        return L_AND_KIND;
    case OR_T:
        return L_OR_KIND;
    case EQ_T:
        return ASSIGN_KIND;
    case COMMA_T:
        return COMMA_KIND;
    default:
        return 0;
    }
}

char *type_name(type_t type)
{
    switch (type)
    {
    case INT_TYPE:
        return "int";
    case CHAR_TYPE:
        return "char";
    default:
        return "?";
    }
}

type_t to_type(token_t token)
{
    switch (token)
    {
    case INT_T:
        return INT_TYPE;
    case CHAR_T:
        return CHAR_TYPE;
    default:
        return 0;
    }
}

int type_size(type_t type)
{
    switch (type)
    {
    case INT_TYPE:
        return 4;
    case CHAR_TYPE:
        return 1;
    default:
        return 0;
    }
}
