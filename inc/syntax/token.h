#ifndef TOKEN
#define TOKEN
typedef enum TokenType
{
    ErrToken,
    IntLiteralToken,
    // keywords
    IdentifierToken,
    TrueToken,
    FalseToken,
    IntToken,
    IfToken,
    ElseToken,
    ForToken,
    WhileToken,
    // scope
    LeftBracket,
    RightBracket,
    LeftBrace,
    RightBrace,
    // binary operator
    PlusToken,
    MinusToken,
    StarToken,
    SlashToken,
    EqualToken,
    // logic
    GreaterToken,
    GreaterEqualToken,
    LessToken,
    LessEqualToken,
    DoubleEqualToken,
    NotEqualToken,
    DoubleLogicAndToken,
    DoubleLogicOrToken,
    LogicAndToken,
    LogicOrToken,
    LogicNotToken,
    // separator
    CommaToken,
    SemiColon,
    // other
    WhiteSpaceToken,
    EndOfFileToken
} TokenType;

char *getTokenTypeValue(TokenType tokenType);

typedef struct Token
{
    TokenType tokenType;
    char *value;
    int length;
} Token;
int isDigit(int ch);
int isLetter(int ch);
int getUnaryTokenPriority(TokenType tokenType);
int getBinaryTokenPriority(TokenType tokenType);
int getAssociation(TokenType tokenType);
Token *createToken(TokenType tokenType, char *value, int length);
Token *createSymbolToken(TokenType tokenType);
void freeToken(Token *token);
#endif