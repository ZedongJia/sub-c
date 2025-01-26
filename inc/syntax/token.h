#ifndef TOKEN
#define TOKEN
typedef enum TokenType
{
    ErrToken,
    IntLiteralToken,
    StringLiteralToken,
    // type-keywords
    IntToken,
    CharToken,
    // keywords
    IdentifierToken,
    TrueToken,
    FalseToken,
    IfToken,
    ElseToken,
    ForToken,
    WhileToken,
    // scope
    LeftParenthesis,
    RightParenthesis,
    LeftBracket,
    RightBracket,
    LeftBrace,
    RightBrace,
    // operator
    PlusToken,
    MinusToken,
    StarToken,
    SlashToken,
    EqualToken,
    // operate
    AccessToken,
    AddressOfToken,
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