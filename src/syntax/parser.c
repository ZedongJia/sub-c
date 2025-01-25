#include "syntax/parser.h"
#include <stdio.h>
#include <stdlib.h>
Parser *createParser()
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    return parser;
}
void freeParser(Parser *parser)
{
    free(parser);
}
Node *parsePrimaryExpression(Parser *parser, Lexer *lexer)
{
    peekToken(lexer);
    switch (lexer->postToken->tokenType)
    {
    case LeftBracket: {
        // ( expression )
        matchToken(lexer, LeftBracket);
        Node *expression = parseExpression(parser, lexer, 0);
        matchToken(lexer, RightBracket);
        return expression;
    }
    case TrueToken:
    case FalseToken: {
        nextToken(lexer);
        return createKeywordExpressionNode(lexer->currToken->tokenType);
    }
    case IntLiteralToken:
    case IdentifierToken: {
        nextToken(lexer);
        return createLiteralExpressionNode(lexer->currToken->tokenType, lexer->currToken->value,
                                           lexer->currToken->length);
    }
    default: {
        printf("\033[35mError: unexpected %s, expect expression\033[0m\n",
               getTokenTypeValue(lexer->postToken->tokenType));
        return createNode(Err, NULL);
    }
    }
}
Node *parseUnaryExpression(Parser *parser, Lexer *lexer, int parentPriority)
{
    Node *expression;
    peekToken(lexer);
    int priority = getUnaryTokenPriority(lexer->postToken->tokenType);
    if (priority != 0 && priority >= parentPriority)
    {
        // right association
        nextToken(lexer);
        TokenType opType = lexer->currToken->tokenType;
        Node *operand = parseExpression(parser, lexer, priority);
        expression = createUnaryOperatorExpressionNode(opType, operand);
    }
    else
        expression = parsePrimaryExpression(parser, lexer);
    return expression;
}
Node *parseExpression(Parser *parser, Lexer *lexer, int parentPriority)
{
    Node *left = parseUnaryExpression(parser, lexer, parentPriority);
    peekToken(lexer);
    TokenType opType = lexer->postToken->tokenType;
    int priority = getBinaryTokenPriority(opType);
    int association = getAssociation(opType);
    while ((priority != 0) && (association ? priority >= parentPriority : priority > parentPriority))
    {
        nextToken(lexer);
        opType = lexer->currToken->tokenType;
        Node *right = parseExpression(parser, lexer, priority);
        left = createBinaryOperatorExpressionNode(left, opType, right); // here use opType, so we store opType
        // peek next token
        peekToken(lexer);
        opType = lexer->postToken->tokenType;
        priority = getBinaryTokenPriority(opType);
        association = getAssociation(opType);
    }
    return left;
}
Node *parseStatement(Parser *parser, Lexer *lexer, int parentPriority)
{
    Node *expression = parseExpression(parser, lexer, parentPriority);
    matchToken(lexer, SemiColon);
    return createStatementNode(expression);
}
Node *parseDeclarationStatement(Parser *parser, Lexer *lexer, int parentPriority)
{
    nextToken(lexer);
    Node *type = createKeywordExpressionNode(lexer->currToken->tokenType);
    Node *expression = parseExpression(parser, lexer, parentPriority);
    matchToken(lexer, SemiColon);
    return createDeclarationNode(type, expression);
}
Node *parseStatements(Parser *parser, Lexer *lexer, int parentPriority, int isGlobal)
{
    Node *head = NULL;
    Node *tail = NULL;
    Node *statement;
    if (!isGlobal)
        matchToken(lexer, LeftBrace);
    peekToken(lexer);
    while ((lexer->postToken->tokenType != EndOfFileToken) &&
           ((isGlobal) || (!isGlobal && lexer->postToken->tokenType != LeftBrace)))
    {
        peekToken(lexer);
        switch (lexer->postToken->tokenType)
        {
        case IntToken:
            statement = parseDeclarationStatement(parser, lexer, parentPriority);
            break;
        default:
            statement = parseStatement(parser, lexer, parentPriority);
            break;
        }
        if (head == NULL)
        {
            head = tail = statement;
        }
        else
        {
            tail->next = statement;
            tail = tail->next;
        }
        peekToken(lexer);
    }
    isGlobal ? matchToken(lexer, EndOfFileToken) : matchToken(lexer, RightBrace);
    return head;
}
Node *parse(Parser *parser, FILE *file)
{
    Lexer *lexer = createLexer(file);
    Node *root = parseStatements(parser, lexer, 0, 1);
    freeLexer(lexer);
    return root;
}