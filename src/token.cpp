#include "value.h"
#include "token.h"

#include <iostream>
#include <optional>
#include <variant>
#include <type_traits>

std::string toString(TokenKind k) {
    switch(k) {
        case TokenKind::END:           return "END";
        case TokenKind::NUMBER:        return "NUMBER";
        case TokenKind::IDENT:         return "IDENT";
        case TokenKind::SYMBOL:        return "SYMBOL";
        case TokenKind::BOOL:          return "BOOL";
        case TokenKind::CHAR:          return "CHAR";
        case TokenKind::STRING:        return "STRING";
        case TokenKind::LAMBDA:        return "LAMBDA";
        case TokenKind::COND:          return "COND";
        case TokenKind::CLAUSE:        return "CLAUSE";
        case TokenKind::QUOTE:         return "QUOTE";
        case TokenKind::QQUOTE:        return "QQUOTE";
        case TokenKind::UNQUOTE:       return "UNQUOTE";
        case TokenKind::UNQUOTESPLICE: return "UNQUOTESPLICE";
        case TokenKind::DEFINE:        return "DEFINE";
        case TokenKind::LIST:          return "LIST";
        case TokenKind::CONS:          return "CONS";
        case TokenKind::LET:           return "LET";
        case TokenKind::LETS:          return "LETS";
        case TokenKind::LETR:          return "LETR";
        case TokenKind::LET_BINDING:   return "LET_BINDING";
        case TokenKind::LPAREN:        return "LPAREN";
        case TokenKind::RPAREN:        return "RPAREN";
        case TokenKind::COLON:         return "COLON";
        case TokenKind::ARROW:         return "ARROW";
        case TokenKind::DOT:           return "DOT";
        case TokenKind::TYPE_IDENT:    return "TYPE_IDENT";
        case TokenKind::TYPE_LIST:     return "TYPE_LIST";
        case TokenKind::VAR_TYPE:      return "VAR_TYPE";
        case TokenKind::TYPE_VAR:      return "TYPE_VAR";
        case TokenKind::PARAM_LIST:    return "PARAM_LIST";
        case TokenKind::RETURN_TYPE:   return "RETURN_TYPE";
        case TokenKind::NOTHING:       return "NOTHING";
        case TokenKind::JUST:          return "JUST";
        case TokenKind::PLACEHOLDER:   return "PLACEHOLDER";
        case TokenKind::MAYBE:         return "MAYBE";
        case TokenKind::FORALL:        return "FORALL";
        case TokenKind::TLAMBDA:       return "TLAMBDA";
        case TokenKind::TAPPLY:        return "TAPPLY";
        case TokenKind::PERFORM:       return "PERFORM";
        case TokenKind::HANDLE:        return "HANDLE";
        case TokenKind::RETURN:        return "RETURN";
        case TokenKind::ERROR:         return "ERROR";
        case TokenKind::RAISE:         return "RAISE";
        case TokenKind::TRY:           return "TRY";
        case TokenKind::CATCH:         return "CATCH";
        case TokenKind::DATA:          return "DATA";
        case TokenKind::MATCH:         return "MATCH";
        case TokenKind::EQ:            return "EQ";
        case TokenKind::EQUALS:        return "EQUALS";
        case TokenKind::SHIFT:         return "SHIFT";
        case TokenKind::RESET:         return "RESET";
        case TokenKind::FORCE:         return "FORCE";
        case TokenKind::DO:            return "DO";
        case TokenKind::NIL:           return "NIL";
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
    os << "TOKEN[ kind=" << toString(tok.kind);
    if (tok.value) {
        os << " value=" << *tok.value;
    }
    //os << " line=" << tok.line << " column=" << tok.column; 
    os << " ]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const TokenList& lst) {
    TokenList temp = lst;
    os << '(';
    while (temp) {
        os << head(temp);
        temp = tail(temp);
        if (temp) { os << ", "; }
    }
    os << ')';
    return os;
}

std::ostream& operator<<(std::ostream& os, const TokenNode& node) {
    std::visit([&os](const auto& v) { os << v; }, node);
    return os;
}

std::size_t size(const TokenNode& node) {
    return std::visit([](const auto& v) -> std::size_t {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, Token>) {
            return 1;
        } else {
            return size(v);
        }
    }, node);
}

std::size_t size(const TokenList& lst) {
    std::size_t count = 0;
    TokenList temp = lst;
    while (temp) {
        ++count;
        temp = tail(temp);
    }
    return count;
}
