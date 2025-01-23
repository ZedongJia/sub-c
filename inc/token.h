#ifndef TOKEN
#define TOKEN
enum TokenType
{
    ErrToken,
    IntLiteralToken,
    // keywords
    IdentifierToken,
    TrueToken,
    FalseToken,
    IntToken,
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
int getAssociation(enum TokenType tokenType);
struct Token *createToken(enum TokenType tokenType, char *value, int valueLength);
struct Token *createSymbolToken(enum TokenType tokenType);
void freeToken(struct Token *token);
#endif