#include "value.h"
#include "token.h"

#include <iostream>
#include <optional>

const char* toString(TokenKind k) {
    switch(k) {
        case TokenKind::END:           return "END          ";
        case TokenKind::NUMBER:        return "NUMBER       ";
        case TokenKind::SYMBOL:        return "SYMBOL       ";
        case TokenKind::BOOLEAN:       return "BOOLEAN      ";
        case TokenKind::CHARACTER:     return "CHARACTER    "; 
        case TokenKind::STRING:        return "STRING       ";
        case TokenKind::LAMBDA:        return "LAMBDA       ";
        case TokenKind::COND:          return "COND         ";
        case TokenKind::QUOTE:         return "QUOTE        ";
        case TokenKind::QQUOTE:        return "QQUOTE       "; 
        case TokenKind::UNQUOTE:       return "UNQUOTE      "; 
        case TokenKind::UNQUOTESPLICE: return "UNQUOTESPLICE";
        case TokenKind::DEFINE:        return "DEFINE       ";
        case TokenKind::LIST:          return "LIST         "; 
        case TokenKind::LETS:          return "LETS         "; 
        case TokenKind::LETR:          return "LETR         "; 
        case TokenKind::LPAREN:        return "LPAREN       "; 
        case TokenKind::RPAREN:        return "RPAREN       "; 
        case TokenKind::DOT:           return "DOT          "; 
        case TokenKind::TYPE:          return "TYPE         "; 
    }
    return "UNKNOWN";
};

Token::Token(TokenKind kind_) { 
    this->kind = kind_;
}

Token::Token(TokenKind kind_, Value value_) {
    this->kind = kind_;
    this->value = value_;
}

std::ostream& operator<<(std::ostream& os, const Token& tok) { 
    os << "TOKEN[ kind=" << toString(tok.kind) << " value= ";
    if (tok.value) { os << *tok.value; }
    else { os << "NULL" ;}
    os << " ]";
    return os;
}

