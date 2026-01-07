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
TokenNode validateParams(TokenNode& params);
TokenNode validateType(TokenNode& types);
TokenNode unwrapIdent(Lexer& lex);
TokenNode parseCond(Lexer& lex);
TokenNode parseLambda(Lexer& lex);
TokenList parseList(Lexer& lex);
TokenNode parseType(Lexer& lex);

TokenNode parse(Lexer& lex);
