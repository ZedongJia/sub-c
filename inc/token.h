#ifndef TOKEN
#define TOKEN
enum TokenType
{
    ErrToken,
    IntLiteralToken,
    // keyword
    IdentifierToken,
    TrueToken,
    FalseToken,
    // scope
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
    // other
    SemiColon,
    WhiteSpaceToken,
    EndOfLineToken
};

char *getTokenTypeValue(enum TokenType tokenType);

struct Token
{
    enum TokenType tokenType;
    char *value;
    int valueLength;
};

int isDigit(int ch);
int isLetter(int ch);
int getUnaryTokenPriority(enum TokenType tokenType);
int getBinaryTokenPriority(enum TokenType tokenType);
struct Token *createToken(enum TokenType tokenType, char *value, int valueLength);
struct Token *createSymbolToken(enum TokenType tokenType);
void freeToken(struct Token *);
#endif