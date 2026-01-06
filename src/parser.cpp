#include "value.h"
#include "cell.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"

#include <iostream>
#include <stack>
#include <stdexcept>
#include <sstream>

TokenList parseList(Lexer& lex) { //called after peeking a LPAREN 
    if (lex.peek(0).kind == TokenKind::LPAREN) { //ensure that current is an LPAREN
        (void) lex.next();      //consume the LPAREN
        std::stack<TokenNode> elems;
        while (lex.peek(0).kind != TokenKind::RPAREN) { //check if the current paren is an RPAREN
           elems.push(parse(lex));
        }
        (void) lex.next();
        if (elems.empty()) {
           return TokenList {};
        }
        TokenList list = cons(elems.top(), TokenList{});
        elems.pop();
        while (!elems.empty()){
            list = cons(elems.top(), list);
            elems.pop();
        }
        return list;
    } else {
        throw std::runtime_error("Lists must start with '('" + toString(lex.peek(0)));
    }
}

TokenNode parseIdent (Lexer& lex) {
    const Token tok = lex.peek(0);
    if (tok.value && std::holds_alternative<Symbol>(tok.value->v)) {
        //(void) lex.next(); remove this line
        const auto& sym = std::get<Symbol>(tok.value->v);
        std::string name = sym.name;
        if (name == "lambda") {
            lex.swapCurrent(Token(TokenKind::LAMBDA, tok.line, tok.column)); 
        } else if (name == "cond") {
            lex.swapCurrent(Token(TokenKind::COND, tok.line, tok.column));
        } else if (name == "let") {
            lex.swapCurrent(Token(TokenKind::LET, tok.line, tok.column));
        } else if (name == "lets") {
            lex.swapCurrent(Token(TokenKind::LETS, tok.line, tok.column));
        } else if (name == "letr") {
            lex.swapCurrent(Token(TokenKind::LETR, tok.line, tok.column));
        } else if (name == "define") {
            lex.swapCurrent(Token(TokenKind::DEFINE, tok.line, tok.column));
        } else if (name == "shift") {
            lex.swapCurrent(Token(TokenKind::SHIFT, tok.line, tok.column));
        } else if (name == "reset") {
            lex.swapCurrent(Token(TokenKind::RESET, tok.line, tok.column));
        } else if (name == "force") {
            lex.swapCurrent(Token(TokenKind::FORCE, tok.line, tok.column));
        } else if (name == "do") {
            lex.swapCurrent(Token(TokenKind::DO, tok.line, tok.column));
        }  else {
            return TokenNode(lex.next()); //token untransformed, consume and resume to the caller
        }
        return parse(lex);
    }
    throw std::runtime_error("Ident with no value or non-symbol value" + toString(tok));
}

TokenNode parseCond(Lexer& lex) {
    if (lex.peek(0).kind == TokenKind::COND) {
        Token cond_ = lex.next();
        std::stack<TokenNode> clauses;
        if (lex.peek(0).kind == TokenKind::LPAREN) {
            while (lex.peek(0).kind == TokenKind::LPAREN) {
                clauses.push(parse(lex));
            }
            if (clauses.size() < 1) { throw std::runtime_error("cond expressions must have at least one clause, found at line:" + std::to_string(cond_.line) + " column:" + std::to_string(cond_.column)); }
            TokenList cond = TokenList {};
            while(!clauses.empty()) {
                if (size(clauses.top()) != 2) { throw std::runtime_error("cond clauses must have two and only two expressions, found at line:" + std::to_string(cond_.line) + " column:" + std::to_string(cond_.column)); }
                TokenList clause = cons(TokenNode{Token(TokenKind::CLAUSE, 0, 0)},cons(clauses.top(), TokenList{}));
                cond = cons(TokenNode{clause}, cond);
                clauses.pop();
            }
            return cons(TokenNode{cond_}, cond);
        }
    }
    throw std::runtime_error("parseCond called on non cond token: " + toString(lex.peek(0)));
}


TokenNode parse(Lexer& lex) {
    switch(lex.peek(0).kind) {
        case TokenKind::NUMBER:
        case TokenKind::BOOL:
        case TokenKind::CHAR:
        case TokenKind::STRING:
        case TokenKind::NIL:
            return TokenNode{lex.next()};
        case TokenKind::LPAREN:
            return TokenNode{parseList(lex)};
        case TokenKind::IDENT:
            return TokenNode{parseIdent(lex)};
        case TokenKind::COND:
            return parseCond(lex);
        default:
            throw std::runtime_error("error" + toString(lex.peek(0)));
    }
}
