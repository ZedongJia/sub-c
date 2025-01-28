#ifndef TOKEN
#define TOKEN
typedef enum TokenType
{
    INT_LITERAL_TOKEN,
    STRING_LITERAL_TOKEN,
    // type-keywords
    INT_TOKEN,
    CHAR_TOKEN,
    // keywords
    IDENTIFIER_TOKEN,
    TRUE_TOKEN,
    FALSE_TOKEN,
    IF_TOKEN,
    ELSE_TOKEN,
    FOR_TOKEN,
    WHILE_TOKEN,
    // scope
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    // operator
    PLUS_TOKEN,
    MINUS_TOKEN,
    STAR_TOKEN,
    SLASH_TOKEN,
    EQUAL_TOKEN,
    // operate
    DIM_TOKEN,
    CALL_TOKEN,
    // logic
    GREATER_TOKEN,
    GREATER_EQUAL_TOKEN,
    LESS_TOKEN,
    LESS_EQUAL_TOKEN,
    DOUBLE_EQUAL_TOKEN,
    NOT_EQUAL_TOKEN,
    DOUBLE_LOGIC_AND_TOKEN,
    DOUBLE_LOGIC_OR_TOKEN,
    LOGIC_AND_TOKEN,
    LOGIC_OR_TOKEN,
    LOGIC_NOT_TOKEN,
    // separator
    COMMA_TOKEN,
    SEMI_COLON_TOKEN,
    // other
    WHITE_SPACE_TOKEN,
    END_OF_FILE_TOKEN
} TokenType;

const char *getTokenTypeValue(TokenType type);

typedef struct Token
{
    TokenType type;
    char *value;
    int length;

    // position
    int line;
    int column;
} Token;
int isDigit(int ch);
int isLetter(int ch);
int getUnaryTokenPriority(TokenType type);
int getBinaryTokenPriority(TokenType type);
int getAssociation(TokenType type);
Token *createToken(TokenType type, char *value, int length, int line, int column);
Token *createSymbolToken(TokenType type, int line, int column);
void freeToken(Token *token);
#endif