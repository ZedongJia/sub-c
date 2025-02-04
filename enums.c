#include "enums.h"

const char *token_name(Token token)
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

const char *kind_name(Kind kind)
{
    switch (kind)
    {
    case LIT_N:
        return "Literal";
    // unary operator
    case ADDR_N:
        return "*";
    case ADDR_OF_N:
        return "&";
    case P_ADD_N:
        return "+";
    case P_SUB_N:
        return "-";
    case NOT_N:
        return "!";
    // binary operator
    case ASSIGN_N:
        return "=";
    case ADD_N:
        return "+";
    case SUB_N:
        return "-";
    case MUL_N:
        return "*";
    case DIV_N:
        return "/";
    case GT_N:
        return ">";
    case GE_N:
        return ">=";
    case LT_N:
        return "<";
    case LE_N:
        return "<=";
    case EQ_N:
        return "==";
    case NE_N:
        return "!=";
    case AND_N:
        return "&&";
    case OR_N:
        return "||";
    case L_AND_N:
        return "&";
    case L_OR_N:
        return "|";
    // multi operator
    case CALL_N:
        return "call(...)";
    case COMMA_N:
        return "...,...";
    case DEC_N:
        return "Declaration";
    case LABEL_N:
        return "Label";
    case JUMP_FALSE_N:
        return "Jump If False";
    case JUMP_N:
        return "Jump";
    case SCOPE_N:
        return "Scope";
    default:
        return "?";
    }
}

int uprior(Kind kind)
{
    switch (kind)
    {
    case P_ADD_N:
    case P_SUB_N:
    case NOT_N:
    case ADDR_N:
    case ADDR_OF_N:
        return 9;
    default:
        return 0;
    }
}

int bprior(Kind kind)
{
    switch (kind)
    {
    case MUL_N:
    case DIV_N:
        return 8;
    case ADD_N:
    case SUB_N:
        return 7;
    case GT_N:
    case GE_N:
    case LT_N:
    case LE_N:
        return 6;
    case EQ_N:
    case NE_N:
        return 5;
    case AND_N:
    case OR_N:
        return 4;
    case L_AND_N:
    case L_OR_N:
        return 3;
    case ASSIGN_N:
        return 2;
    case COMMA_N:
        return 1;
    default:
        return 0;
    }
}

int assoc(Kind kind)
{
    switch (kind)
    {
    case ASSIGN_N:
        return 1;
    default:
        return 0;
    }
}

Kind to_ukind(Token token)
{
    switch (token)
    {
    case PLUS_T:
        return P_ADD_N;
    case MIN_T:
        return P_SUB_N;
    case NOT_T:
        return NOT_N;
    case AND_T:
        return ADDR_OF_N;
    case STAR_T:
        return ADDR_N;
    default:
        return 0;
    }
}

Kind to_bkind(Token token)
{
    switch (token)
    {
    case STAR_T:
        return MUL_N;
    case SLASH_T:
        return DIV_N;
    case PLUS_T:
        return ADD_N;
    case MIN_T:
        return SUB_N;
    case GT_T:
        return GT_N;
    case GE_T:
        return GE_N;
    case LT_T:
        return LT_N;
    case LE_T:
        return LE_N;
    case D_EQ_T:
        return EQ_N;
    case NE_T:
        return NE_N;
    case D_AND_T:
        return AND_N;
    case D_OR_T:
        return OR_N;
    case AND_T:
        return L_AND_N;
    case OR_T:
        return L_OR_N;
    case EQ_T:
        return ASSIGN_N;
    case COMMA_T:
        return COMMA_N;
    default:
        return 0;
    }
}

char *type_name(Type valueType)
{
    switch (valueType)
    {
    case INT_TYPE:
        return "int";
    case CHAR_TYPE:
        return "char";
    default:
        return "?";
    }
}

Type to_type(Token tokenType)
{
    switch (tokenType)
    {
    case INT_T:
        return INT_TYPE;
    case CHAR_T:
        return CHAR_TYPE;
    default:
        return 0;
    }
}

int type_size(Type type)
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
