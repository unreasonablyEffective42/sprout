#include "value.h"
#include "cell.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"

#include <iostream>
#include <stack>
#include <stdexcept>
#include <sstream>

TokenList parseList(Lexer& lex) { //called after consuming a LPAREN   
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
}

void promoteIdent(Lexer& lex) {
    const Token tok = lex.peek(0);
    if (tok.value && std::holds_alternative<Symbol>(tok.value->v)) { 
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
            return;
        }
    } else {
        throw std::runtime_error("Ident with no value or non-symbol value" + toString(tok));
    }
}

TokenNode parseCond(Lexer& lex) { 
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
    throw std::runtime_error("bad cond form, clauses must be lists of two expressions, found no list at line:" + std::to_string(lex.peek(0).line) + " column:" +std::to_string(lex.peek(0).column));
}

TokenNode parseLambda(Lexer& lex) {
    Token lambda_ = lex.next();
    if (lex.peek(0).kind == TokenKind::LPAREN) {
        TokenNode parameters = parse(lex);
        parameters = validateParams(parameters);
        TokenNode body = parse(lex);
        if (lex.peek(0).kind != TokenKind::RPAREN) {
            throw std::runtime_error(
                "lambda expressions may only have one body expression, found:" + toString(lex.peek(0))
            );
        }
        return TokenNode{
            cons(TokenNode{lambda_},
                cons(parameters, cons(body, TokenList{})))
        };
    }
    throw std::runtime_error(
        "lambda must be followed by parameter list, found:" + toString(lex.peek(0))
    );
}

//horrible state machine pattern <symbol colon type ...> arrow type
TokenNode validateParams(TokenNode& params) {
    int state = 0;
    const TokenList& lst = asTokenList(params);
    TokenListIterator it(lst);
    TokenListIterator end(TokenList{});
    std::stack<Token> argument;
    std::stack<TokenList> arguments;
    TokenList arglist;
    while(it != end) {
        const TokenNode& node = *it;
        if (isTokenNodeList(node)) {
            throw std::runtime_error("unexpected list in param list");
        } else {
            const Token& tok = std::get<Token>(node);
            switch(state) {
                case 0: //expect TokenKind::SYMBOL
                    if (tok.kind != TokenKind::SYMBOL) { throw std::runtime_error("param list must begin with a symbol, found:" + toString(tok)); }
                    state = 2;
                    argument.push(tok);
                    it++;
                    continue;
                case 1: //expect TokenKind::SYMBOL or TokenKind::ARROW
                    arglist = TokenList {};
                    while (!argument.empty()){
                        arglist = cons(TokenNode{argument.top()}, arglist);
                        argument.pop();
                    }
                    arguments.push(arglist);
                    switch(tok.kind) {
                        case TokenKind::SYMBOL:
                            state++;
                            argument.push(tok);
                            it++;
                            continue;
                        case TokenKind::ARROW:
                            state = 4;
                            it++;
                            continue;
                        default:
                            throw std::runtime_error("expected argument or arrow, found:" + toString(tok));
                    }
                    break;
                case 2: //expect TokenKind::COLON
                    if (tok.kind != TokenKind::COLON) {throw std::runtime_error("expected colon, found:" + toString(tok));}
                    state++;
                    it++;
                    continue;
                case 3: //expect TokenKind::TYPE_IDENT
                    if (tok.kind != TokenKind::TYPE_IDENT) {throw std::runtime_error("expected type, found:" + toString(tok));}
                    state=1;
                    argument.push(tok);
                    it++;
                    continue;
                case 4:
                    if (tok.kind != TokenKind::TYPE_IDENT) {
                        throw std::runtime_error("expected type, found:" + toString(tok));
                    }
                    Token ret = Token(TokenKind::RETURN_TYPE, *tok.value, tok.line, tok.column);
                    TokenList params_ = cons(TokenNode{ret}, TokenList{});

                    while (!arguments.empty()) {
                        params_ = cons(TokenNode{arguments.top()}, params_);
                        arguments.pop();
                    }

                    it++;
                    if (it != end) {
                        throw std::runtime_error("param list did not terminate in a type or has more than one in lambda at line: " + std::to_string(tok.line));
                    }
                    TokenList wrapped = cons(TokenNode{Token(TokenKind::PARAM_LIST, 0, 0)}, params_);
                    return TokenNode{wrapped};        
            }
        }
    }
    throw std::runtime_error("invalid param list"); //idk how we get here but okay
}

TokenNode unwrapIdent(Lexer& lex) {
    Token temp = lex.next();
    if (!temp.value) {
        throw std::runtime_error("attempted to unwrap identifier with no value: " + toString(temp));
    }
    const auto& sym = std::get<Symbol>(temp.value->v);
    return TokenNode{Token(TokenKind::SYMBOL, Value(sym.name), temp.line, temp.column)};
}

TokenNode parse(Lexer& lex) {
    switch(lex.peek(0).kind) {
        case TokenKind::NUMBER:
        case TokenKind::BOOL:
        case TokenKind::CHAR:
        case TokenKind::STRING:
        case TokenKind::NIL:
        case TokenKind::COLON:
        case TokenKind::ARROW:
        case TokenKind::TYPE_IDENT:
            return TokenNode{lex.next()};
        case TokenKind::LPAREN:
            (void) lex.next(); //consume LPAREN 
            if (lex.peek(0).kind == TokenKind::IDENT) { promoteIdent(lex); }
            switch(lex.peek(0).kind) {
                case TokenKind::COND:
                    return parseCond(lex);
                case TokenKind::LAMBDA:
                    return parseLambda(lex);
                default :
                    return TokenNode{parseList(lex)};
            }
        case TokenKind::IDENT:
            return unwrapIdent(lex);
        default:
            throw std::runtime_error("error" + toString(lex.peek(0)));
    }
}
