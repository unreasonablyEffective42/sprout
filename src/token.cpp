#include "value.h"
#include "token.h"

#include <iostream>
#include <optional>

const char* toString(TokenKind k) {
    switch(k) {
        case TokenKind::END:           return "END          ";
        case TokenKind::NUMBER:        return "NUMBER       ";
        case TokenKind::IDENT:         return "IDENT        ";
        case TokenKind::BOOL:          return "BOOL         ";
        case TokenKind::CHAR:          return "CHAR         "; 
        case TokenKind::STRING:        return "STRING       ";
        case TokenKind::LAMBDA:        return "LAMBDA       ";
        case TokenKind::COND:          return "COND         ";
        case TokenKind::CLAUSE:        return "CLAUSE       ";
        case TokenKind::QUOTE:         return "QUOTE        ";
        case TokenKind::QQUOTE:        return "QQUOTE       "; 
        case TokenKind::UNQUOTE:       return "UNQUOTE      "; 
        case TokenKind::UNQUOTESPLICE: return "UNQUOTESPLICE";
        case TokenKind::DEFINE:        return "DEFINE       ";
        case TokenKind::LIST:          return "LIST         "; 
        case TokenKind::CONS:          return "CONS         ";
        case TokenKind::LET:           return "LET          ";
        case TokenKind::LETS:          return "LETS         "; 
        case TokenKind::LETR:          return "LETR         "; 
        case TokenKind::LPAREN:        return "LPAREN       "; 
        case TokenKind::RPAREN:        return "RPAREN       "; 
        case TokenKind::COLON:         return "COLON        ";
        case TokenKind::ARROW:         return "ARROW        ";
        case TokenKind::DOT:           return "DOT          "; 
        case TokenKind::TYPE_IDENT:    return "TYPE_IDENT   ";
        case TokenKind::VAR_TYPE:      return "VAR_TYPE     ";
        case TokenKind::RETURN_TYPE:   return "RETURN_TYPE  ";
        case TokenKind::SHIFT:         return "SHIFT        ";
        case TokenKind::RESET:         return "RESET        ";
        case TokenKind::FORCE:         return "FORCE        ";
        case TokenKind::DO:            return "DO           ";
        case TokenKind::NIL:           return "NIL          ";
    }
    return "UNKNOWN";
};
Token::Token() = default;

Token::Token(TokenKind kind_, int line_, int column_) { 
    this->kind   = kind_;
    this->line   = line_;
    this->column = column_;
}

Token::Token(TokenKind kind_, Value value_, int line_, int column_) {
    this->kind   = kind_;
    this->value  = value_;
    this->line   = line_;
    this->column = column_; 
}

bool operator==(const Token& a, const Token& b) {
    return a.kind == b.kind && a.value == b.value;
}

std::ostream& operator<<(std::ostream& os, const Token& tok) { 
    os << "TOKEN[ kind=" << toString(tok.kind) << " value=";
    if (tok.value) { os << *tok.value; }
    else { os << "NULL"; }
    os << " line=" << tok.line << " column=" << tok.column << " ]";
    return os;
}
