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
TokenList parseList(Lexer& lex);
TokenNode parse(Lexer& lex);
