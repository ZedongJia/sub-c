#include "node.h"

const char *kindName(Kind kind)
{
    switch (kind)
    {
    case LIT_N:
        return "Literal";
    // unary operator
    case ADDR_N:
        return "*()";
    case ADDR_OF_N:
        return "&()";
    case P_ADD_N:
        return "+()";
    case P_SUB_N:
        return "-()?";
    case NOT_N:
        return "!()";
    // binary operator
    case ADD_N:
        return "()+()";
    case SUB_N:
        return "()-()";
    case MUL_N:
        return "()*()";
    case DIV_N:
        return "()/()";
    case ASSIGN_N:
        return "()=()";
    case GT_N:
        return "()>()";
    case GE_N:
        return "()>=()";
    case LT_N:
        return "()<()";
    case LE_N:
        return "()<=()";
    case EQ_N:
        return "()==()";
    case NE_N:
        return "()!=()";
    case AND_N:
        return "()&&()";
    case OR_N:
        return "()||()";
    case L_AND_N:
        return "()&()";
    case L_OR_N:
        return "()|()";
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

int unaryPriority(Kind kind)
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

int binaryPriority(Kind kind)
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

Kind unaryNode(Token token)
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

Kind binaryNode(Token token)
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
