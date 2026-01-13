#include "value.h"
#include "cell.h"
#include "token.h"
#include "lexer.h"

#include <iostream>
#include <stack>
#include <stdexcept>

inline constexpr std::array<TokenKind, 6> atomics = {
    TokenKind::NUMBER, 
    TokenKind::CHAR,
    TokenKind::STRING,
    TokenKind::BOOL,
    TokenKind::NIL,
};

inline bool isAtom(const Token& tok) {
    switch (tok.kind) {
        case TokenKind::NUMBER:
        case TokenKind::CHAR:
        case TokenKind::STRING:
        case TokenKind::BOOL:
        case TokenKind::NIL:
            return true;
        default:
            return false;
    }
}
void promoteIdent(Lexer& lex);
TokenNode validateParams(const TokenNode& params);
TokenNode validateTypeList(const TokenNode& types);
TokenNode validateTypeParams(const TokenNode& params);
TokenNode validateForall(const TokenNode& forall);
TokenList validateDottedList(const TokenList& lst);
TokenNode validatePatternClause(const TokenNode& patternClause);
bool validateQuote(const TokenNode& node, int depth);
bool validateQuoteList(const TokenList& lst, int depth);


TokenNode unwrapIdent(Lexer& lex);
TokenNode parseCond(Lexer& lex);
TokenNode parseLambda(Lexer& lex);
TokenNode parseTypeLambda(Lexer& lex);
TokenList parseList(Lexer& lex);
TokenNode parseLet(Lexer& lex);
TokenNode parseDefine(Lexer& lex);
TokenNode parseQuote(Lexer& lex);
TokenNode parseBinding(Lexer& lex);
TokenNode parseLet(Lexer& lex);
TokenNode parseMatch(Lexer& lex);
TokenNode parseTypeApplication(Lexer& lex);
TokenNode parse(Lexer& lex);
