#include "value.h"
#include "cell.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
  
#include <iostream>
#include <stack>
#include <stdexcept>
#include <sstream>

TokenList parseList(Lexer& lex) { //called after consuming a LPAREN   
    std::stack<TokenNode> elems;
    Token err = lex.peek(0);
    while (lex.peek(0).kind != TokenKind::RPAREN && lex.peek(0).kind != TokenKind::END) { //check if the current paren is an RPAREN
        elems.push(parse(lex));
    }
    if (lex.peek(0).kind == TokenKind::END) {
        throw std::runtime_error("unterminated list found at line:" + std::to_string(err.line) + " column:" +std::to_string(err.column) );
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
        } else if (name == "forall") {
            lex.swapCurrent(Token(TokenKind::FORALL, tok.line, tok.column));
        } else if (name == "tlambda") {
            lex.swapCurrent(Token(TokenKind::TLAMBDA, tok.line, tok.column));
        } else if (name == "tapply") {
            lex.swapCurrent(Token(TokenKind::TAPPLY, tok.line, tok.column));
        } else if (name == "perform") {
            lex.swapCurrent(Token(TokenKind::PERFORM, tok.line, tok.column));
        } else if (name == "handle") {
            lex.swapCurrent(Token(TokenKind::HANDLE, tok.line, tok.column));
        } else if (name == "return") {
            lex.swapCurrent(Token(TokenKind::RETURN, tok.line, tok.column));
        } else if (name == "error") {
            lex.swapCurrent(Token(TokenKind::ERROR, tok.line, tok.column));
        } else if (name == "raise") {
            lex.swapCurrent(Token(TokenKind::RAISE, tok.line, tok.column));
        } else if (name == "try") {
            lex.swapCurrent(Token(TokenKind::TRY, tok.line, tok.column));
        } else if (name == "catch") {
            lex.swapCurrent(Token(TokenKind::CATCH, tok.line, tok.column));
        } else if (name == "eq?") {
            lex.swapCurrent(Token(TokenKind::EQ, tok.line, tok.column));
        } else if (name == "equal?") {
            lex.swapCurrent(Token(TokenKind::EQUALS, tok.line, tok.column));
        } else if (name == "match") {
            lex.swapCurrent(Token(TokenKind::MATCH, tok.line, tok.column));
        } else if (name == "data") {
            lex.swapCurrent(Token(TokenKind::DATA, tok.line, tok.column));
        } else {
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
        (void) lex.next(); //consume closing RPAREN;
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
        (void) lex.next(); //consume closing rparen 
        return TokenNode{
            cons(TokenNode{lambda_},
                cons(parameters, cons(body, TokenList{})))
        };
    }
    throw std::runtime_error(
        "lambda must be followed by parameter list, found:" + toString(lex.peek(0))
    );
}

//horrible state machine pattern <symbol colon <type or symbol> ...> arrow <type or symbol>
TokenNode validateParams(const TokenNode& params) {
    int state = 0;
    const TokenList& lst = asTokenList(params);
    TokenListIterator it(lst);
    TokenListIterator end(TokenList{});
    std::stack<TokenNode> argument;
    std::stack<TokenList> arguments;
    TokenList arglist;
    while(it != end) {
        const TokenNode& node = *it;
        if (isTokenNodeList(node)) { // if TokenList and state == 3 validate the list and push it
            switch(state) {
                case 3:
                    argument.push(validateTypeList(node));
                    it++;
                    state = 1;
                    continue;
                case 4:{
                    TokenNode node_ = validateTypeList(node);
                    const Token& errtok = std::get<Token>(node_);
                    auto ast = std::make_shared<AstNode>(node_);
                    Token ret = Token(TokenKind::RETURN_TYPE, Value(ast), errtok.line, errtok.column);
                    TokenList params_ = cons(TokenNode{ret}, TokenList{});

                    while (!arguments.empty()) {
                        params_ = cons(TokenNode{arguments.top()}, params_);
                        arguments.pop();
                    }
                    it++;
                    if (it != end) {
                        throw std::runtime_error("param list did not terminate in a typelist or has more than one in lambda at line: " + toString(ret));
                    }
                    TokenList wrapped = cons(TokenNode{Token(TokenKind::PARAM_LIST, 0, 0)}, params_);
                    return TokenNode{wrapped};        
                }
                default:
                    std::ostringstream oss;
                    oss << node;
                    std::string msg = ( state == 0 ? "param list must begin with a symbol, found:" : ( state == 1 ? "expected argument or arrow, found:" : "expected colon, found:"));
                    throw std::runtime_error(msg + oss.str());
            } 
        } else {
            const Token& tok = std::get<Token>(node);
            switch(state) {
                case 0: //expect TokenKind::SYMBOL
                    if (tok.kind != TokenKind::SYMBOL) { throw std::runtime_error("param list must begin with a symbol, found:" + toString(tok)); }
                    state = 2;
                    argument.push(TokenNode{tok});
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
                            argument.push(TokenNode{tok});
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
                case 3: //expect TokenKind::TYPE_IDENT or SYMBOL
                    if (tok.kind == TokenKind::TYPE_IDENT) {
                        state=1;
                        argument.push(TokenNode{tok});
                        it++;
                        continue;
                    } else if (tok.kind == TokenKind::SYMBOL) {
                        state=1;
                        argument.push(TokenNode{Token(TokenKind::TYPE_VAR, *tok.value, tok.line, tok.column)});
                        it++;
                        continue;
                    } else {
                        throw std::runtime_error("expected type, found:" + toString(tok));
                    }
                case 4:

                    if (tok.kind != TokenKind::TYPE_IDENT && tok.kind != TokenKind::SYMBOL) {
                        throw std::runtime_error("expected type, found:" + toString(tok));
                    }
                    Token temp = tok;
                    if (tok.kind == TokenKind::SYMBOL) {
                        temp = Token(TokenKind::TYPE_VAR, *tok.value, tok.line, tok.column);
                    }
                    auto ast = std::make_shared<AstNode>(TokenNode{temp});
                    Token ret = Token(TokenKind::RETURN_TYPE, Value(ast), tok.line, tok.column);
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

// (type arrow (type arrow type) arrow type)
TokenNode validateTypeList(const TokenNode& types) {
    if(isTokenNodeToken(types)){
        const Token& type = std::get<Token>(types);
        if(type.kind == TokenKind::TYPE_IDENT && type.value.has_value() && isAstPtr(*type.value)){
            return types;
        } 
        throw std::runtime_error("expected type list, found: " + toString(type));
    }
    int state = 0;
    const TokenList& lst = asTokenList(types);
    const Token& headtok = std::get<Token>(head(lst));
    if (headtok.kind == TokenKind::FORALL) {
        return validateForall(types);
    }
    TokenListIterator it(lst);
    TokenListIterator end(TokenList{});
    std::stack<TokenNode> typeNodes;
    const Token* errtok = nullptr;
    while (it != end) {
        const TokenNode& node = *it;
        switch (state) {
            case 0: // expect a type, a symbol or a TokenList
                if (isTokenNodeList(node)) {
                    typeNodes.push(validateTypeList(node));
                    it++;
                    state = 1;
                    continue;
                } else {
                    const Token& tok = std::get<Token>(node);
                    if (tok.kind == TokenKind::TYPE_IDENT) {
                        typeNodes.push(node);
                        it++;
                        state = 1;
                        continue;
                    } else if (tok.kind == TokenKind::SYMBOL) {
                        typeNodes.push(TokenNode{Token(TokenKind::TYPE_VAR, *tok.value, tok.line, tok.column)});
                        it++;
                        state = 1;
                        continue;
                    } else {
                        throw std::runtime_error("expected type in type list, found: " + toString(tok));
                    }
                } 
                break;
            case 1: // expect an arrow, or a type/tokenlist  
                if (isTokenNodeToken(node)) {
                    const Token& tok = std::get<Token>(node);
                    if (tok.kind == TokenKind::ARROW) {
                        it++;
                        state = 0;
                        errtok = &tok;
                        continue;
                    }
                     else if (tok.kind == TokenKind::TYPE_IDENT){
                         typeNodes.push(node);
                         it++;
                         state = 2;
                         continue;
                    } else if (tok.kind == TokenKind::SYMBOL) {
                        typeNodes.push(TokenNode{Token(TokenKind::TYPE_VAR, *tok.value, tok.line, tok.column)});
                        it++;
                        state = 2;
                        continue;
                    }
                } else if (isTokenNodeList(node)) {
                        typeNodes.push(validateTypeList(node));
                        it++;
                        state = 2;
                        continue;
                } 
                break;
            case 2: //expect int 
                if (isTokenNodeToken(node)) {
                    const Token& tok = std::get<Token>(node);
                    if (tok.kind == TokenKind::NUMBER) {
                        typeNodes.push(node);
                        it++;
                        state = 1;
                        continue;
                    }
                    throw std::runtime_error("expected number in composite type, found:" + toString(tok));
                } 
                {
                    std::ostringstream oss;
                    oss<<node;
                    throw std::runtime_error("expected number in composite type, found:"  + oss.str());
                }
                break;
        }
    }
    if (state == 0) {
        throw std::runtime_error(
            "typeList did not terminate in a type, found " +
            (errtok ? toString(*errtok) : std::string("<none>"))
        );
    }

    TokenList temp = TokenList{};
    while (!typeNodes.empty()) {
        temp = cons(typeNodes.top(), temp);
        typeNodes.pop();
    }
    auto ast = std::make_shared<AstNode>(TokenNode{temp});
    return TokenNode{Token(TokenKind::TYPE_IDENT,Value(ast),0,0)};
}

TokenNode unwrapIdent(Lexer& lex) {
    Token temp = lex.next();
    if (!temp.value) {
        throw std::runtime_error("attempted to unwrap identifier with no value: " + toString(temp));
    }
    const auto& sym = std::get<Symbol>(temp.value->v);
    if (sym.name == "else") {
        return TokenNode{Token(TokenKind::BOOL, Value(true), temp.line, temp.column)};
    }
    return TokenNode{Token(TokenKind::SYMBOL, Value(sym.name), temp.line, temp.column)};
}

TokenNode parseDefine(Lexer& lex) {
    Token root = lex.next();
    if (lex.peek(0).kind != TokenKind::IDENT) {
        throw std::runtime_error("expected symbol after define, found:" + toString(lex.peek(0)));
    }
    const TokenNode sym = parse(lex);
    switch (lex.peek(0).kind) {
        case TokenKind::COLON: { // (define x:int expr) or (define x:(vec int 3) expr)
            (void) lex.next(); 
            TokenNode type = parse(lex);
            TokenNode expr = parse(lex);

            if (isTokenNodeList(type)) {
                type = validateTypeList(type);
            }
            (void) lex.next(); //consume closing rparen
            return TokenNode{cons(TokenNode{root}, cons(sym, cons(type, cons(expr, TokenList{}))))};
        }

        case TokenKind::LPAREN: { // (define foo (params ...) expr) or (define foo (lambda ...))
            Token temp = lex.peek(1);
            if (temp.kind == TokenKind::IDENT &&
                temp.value &&
                std::get<Symbol>(temp.value->v).name == "lambda") {
                TokenNode lambda = parse(lex);
                (void) lex.next(); //consume closing rparen
                return TokenNode{cons(TokenNode{root}, cons(sym, cons(lambda, TokenList{})))};
            }
            TokenNode type = parse(lex);
            TokenNode expr = parse(lex);
            if (isTokenNodeList(type)) {
                type = validateParams(type);
                auto ast = std::make_shared<AstNode>(type);
                type = TokenNode{Token(TokenKind::TYPE_IDENT, Value(ast), 0, 0)};
            }
            (void) lex.next(); //consume closing rparen
            return TokenNode{cons(TokenNode{root}, cons(sym, cons(type, cons(expr, TokenList{}))))};
        }
        default:
            throw std::runtime_error("expected type or closure, found:" + toString(lex.peek(0)));
    }
}

bool validateQuoteList(const TokenList& lst, int depth) {
    for (TokenListIterator it(lst), end(TokenList{}); it != end; ++it) {
        if (!validateQuote(*it, depth)) return false;
    }
    return true;
}

bool validateQuote(const TokenNode& node, int depth) {
    if (isTokenNodeToken(node)) {
        const Token& tok = std::get<Token>(node);
        if (tok.kind == TokenKind::UNQUOTE || tok.kind == TokenKind::UNQUOTESPLICE) {
            return depth > 0;
        }
        return true;
    }
    const TokenList& lst = asTokenList(node);
    if (!lst) return true;

    // If list begins with quote-like token, adjust depth for its argument.
    const TokenNode& headNode = head(lst);
    if (isTokenNodeToken(headNode)) {
        const Token& headTok = std::get<Token>(headNode);
        int nextDepth = depth;
        if (headTok.kind == TokenKind::QQUOTE) nextDepth = depth + 1;
        else if (headTok.kind == TokenKind::UNQUOTE || headTok.kind == TokenKind::UNQUOTESPLICE) {
            if (depth == 0) return false;
            nextDepth = depth - 1;
        }
        // validate the rest of the list at nextDepth
        return validateQuoteList(tail(lst), nextDepth);
    }
    return validateQuoteList(lst, depth);
}


TokenNode parseQuote(Lexer& lex) {
    Token root = lex.next();
    TokenNode quoted = parse(lex);
    int depth = (root.kind == TokenKind::QQUOTE) ? 1 : 0;
    if ((root.kind == TokenKind::UNQUOTE || root.kind == TokenKind::UNQUOTESPLICE) && depth == 0) {
        throw std::runtime_error("unquote outside quasiquote" + toString(root));
    }
    if (validateQuote(quoted,depth)) { return TokenNode{cons(TokenNode{root}, cons(quoted, TokenList {}))}; }
    throw std::runtime_error("unreachable");
} 
/*
(let ((x:int 0)
      (y:(int->int) (lambda (z:int -> int) expr)))
  body)
*/
TokenNode parseBinding(Lexer& lex){
    Token lparen = lex.next();
    TokenNode sym, type, val;
    if (lex.peek(0).kind == TokenKind::IDENT){
        sym = parse(lex); 
        if (lex.peek(0).kind == TokenKind::COLON){
            (void) lex.next();
            if (lex.peek(0).kind == TokenKind::TYPE_IDENT){
                type = parse(lex);
            } else if (lex.peek(0).kind == TokenKind::LPAREN) {
                type = parse(lex);
                type = validateTypeList(type);
            } else if (lex.peek(0).kind == TokenKind::IDENT){
                TokenNode t = parse(lex); 
                const Token& tok = std::get<Token>(t);
                if (tok.kind == TokenKind::SYMBOL && tok.value){
                    type = TokenNode{Token(TokenKind::TYPE_VAR, *tok.value, tok.line, tok.column)};
                } else {
                    throw std::runtime_error("expected a type var in type position, found:" + toString(tok));
                }

            } else {
                throw std::runtime_error("expected type, found:" + toString(lex.peek(0)));
            }
            val = parse(lex);
            TokenNode binding = TokenNode{cons(sym, cons(type, cons(val, TokenList{})))};
            auto ast = std::make_shared<AstNode>(binding);
            Token root = Token(TokenKind::LET_BINDING, Value(ast), lparen.line,lparen.column);
            if (lex.peek(0).kind == TokenKind::RPAREN) {
                (void) lex.next();
                return TokenNode(root);
            }
            throw std::runtime_error("expected ')' to close binding, found:" + toString(lex.peek(0)));
        }
        throw std::runtime_error("expected ':' in binding type, found:" + toString(lex.peek(0)));
    }
    throw std::runtime_error("bindings must start with a symbol, found:" + toString(lex.peek(0)));
}

TokenNode parseLet(Lexer& lex) {
    Token root = lex.next();
    TokenNode expr, name = TokenNode{Token(TokenKind::SYMBOL,0,0)};
    std::stack<TokenNode> bindings;
    TokenList bindings_ = TokenList {};
    if (lex.peek(0).kind == TokenKind::IDENT) { //named let branch
        name = parse(lex);
        Token name_;
        if (isTokenNodeToken(name)){
            name_ = std::get<Token>(name);
        } else {
            std::ostringstream oss;
            oss << name;
            throw std::runtime_error("expected a name for named let, found:" + oss.str());
        }
       
        if (name_.kind == TokenKind::SYMBOL) {
            if (lex.peek(0).kind == TokenKind::LPAREN){
                (void) lex.next();
                while (lex.peek(0).kind != TokenKind::RPAREN) {
                    bindings.push(parseBinding(lex));
                }
                (void) lex.next(); //consume RPAREN
                while (!bindings.empty()){
                    bindings_ = cons(bindings.top(), bindings_);
                    bindings.pop();
                }
            } else {
                throw std::runtime_error("named " + toString(root.kind) + " not followed with bindings list, found:" + toString(lex.peek(0)));
            } 
        }
    } else if (lex.peek(0).kind == TokenKind::LPAREN) { //regular let branch 
        (void) lex.next();
        while (lex.peek(0).kind != TokenKind::RPAREN) {
            bindings.push(parseBinding(lex));
        }
        (void) lex.next(); //consume RPAREN
        while (!bindings.empty()){
            bindings_ = cons(bindings.top(), bindings_);
            bindings.pop();
        }
    } else {
        throw std::runtime_error(toString(root.kind) +" bindings must begin with a name or a bindings list, found:" + toString(lex.peek(0)));
    }
    expr = parse(lex);
    (void) lex.next(); //consume closing rparen
    return TokenNode{cons(TokenNode{root},cons(name, cons(TokenNode{bindings_}, cons(expr, TokenList{}))))};
}

TokenNode validateTypeParams(const TokenNode& params) {
    const TokenList& lst = asTokenList(params);
    TokenListIterator it(lst);
    TokenListIterator end(TokenList{});
    std::stack<Token> parameters;
    while(it != end ) {
        const TokenNode& node = *it;
        if (isTokenNodeToken(node)){
            const Token& tok = std::get<Token>(node);
            if (tok.kind == TokenKind::SYMBOL && tok.value) {
                parameters.push(Token(TokenKind::TYPE_VAR, *tok.value, tok.line, tok.column));
                it++;
                continue;
            }
            throw std::runtime_error("expected type variable in parameters list for type lambda, found:" + toString(tok));
        }
        std::ostringstream oss;
        oss << node;
        throw std::runtime_error("expected flat parameter list in type lambda, found" + oss.str());
    }
    TokenList params_ = TokenList {};
    if (parameters.empty()){
        throw std::runtime_error("type lambdas cannot have no parameters");
    }
    while (!parameters.empty()) {
        params_ = cons(TokenNode{parameters.top()}, params_);
        parameters.pop();
    }
    auto ast = std::make_shared<AstNode>(TokenNode{params_});
    return TokenNode{Token(TokenKind::TYPE_PARAM_LIST, Value(ast), 0, 0)};
   
}

TokenNode parseTypeLambda(Lexer& lex) {
    Token tlambda_ = lex.next();
    if (lex.peek(0).kind == TokenKind::LPAREN) {
        TokenNode parameters = parse(lex);
        parameters = validateTypeParams(parameters);
        TokenNode body = parse(lex);
        if (lex.peek(0).kind != TokenKind::RPAREN) {
            throw std::runtime_error(
                "type lambda expressions may only have one body expression, found:" + toString(lex.peek(0))
            );
        }
        (void) lex.next();
        return TokenNode{cons(TokenNode{tlambda_}, cons(parameters, cons(body, TokenList {})))};
    }
    throw std::runtime_error("type lambda expressions must be followed by parameter list, found:" + toString(lex.peek(0)));
}

TokenNode parseTypeApplication(Lexer& lex) {
    Token tapply = lex.next();
    TokenNode expr = parse(lex);

    std::stack<TokenNode> types;
    if (lex.peek(0).kind == TokenKind::RPAREN) {
        throw std::runtime_error("attempted to apply type lambda to no types in:" + toString(tapply));
    }

    while (lex.peek(0).kind != TokenKind::RPAREN) {
        TokenNode ty = parse(lex); // handles IDENT -> SYMBOL
        if (isTokenNodeList(ty)) {
            ty = validateTypeList(ty);
        } else {
            const Token& tok = std::get<Token>(ty);
            if (tok.kind == TokenKind::SYMBOL && tok.value) {
                ty = TokenNode{Token(TokenKind::TYPE_VAR, *tok.value, tok.line, tok.column)};
            } else if (tok.kind != TokenKind::TYPE_IDENT) {
                throw std::runtime_error("expected type in tapply, found:" + toString(tok));
            }
        }
        types.push(ty);
    }

    (void) lex.next(); // consume ')'

    TokenList types_ = TokenList{};
    while (!types.empty()) {
        types_ = cons(types.top(), types_);
        types.pop();
    }

    return TokenNode{cons(TokenNode{tapply}, cons(expr, types_))};
}

TokenNode validateForall(const TokenNode& forall) {
    const TokenList& lst = asTokenList(forall);
    TokenListIterator it(lst);
    TokenListIterator end(TokenList{});
    Token forall_ = std::get<Token>(*it);
    if (forall_.kind != TokenKind::FORALL) {
        throw std::runtime_error("encountered: " + toString(forall_) + " in forall");
    }
    it++;
    if (!isTokenNodeList(*it)) { throw std::runtime_error("expected type variable list, found:" + toString(std::get<Token>(*it))); }
    TokenNode params = validateTypeParams(*it);
    it++;
    if (!isTokenNodeList(*it)) { throw std::runtime_error("expected type list, found:" + toString(std::get<Token>(*it))); }
    TokenNode body = validateTypeList(*it);
    it++;
    if (it != end) { 
        std::ostringstream oss;
        oss << *it;
        throw std::runtime_error("forall type expresssions may only have one body expression, found:" + oss.str());
    }
    auto ast = std::make_shared<AstNode>(TokenNode{cons(TokenNode{forall_}, cons(params, cons(body, TokenList {})))});
    return TokenNode{Token(TokenKind::TYPE_IDENT, Value(ast), forall_.line, forall_.column)};
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
        case TokenKind::DOT:
        case TokenKind::TYPE_IDENT:
        case TokenKind::FORALL:
            return TokenNode{lex.next()};
        case TokenKind::LPAREN:
            (void) lex.next(); //consume LPAREN 
            if (lex.peek(0).kind == TokenKind::IDENT) { promoteIdent(lex); }
            switch(lex.peek(0).kind) {
                case TokenKind::COND:
                    return parseCond(lex);
                case TokenKind::LAMBDA:
                    return parseLambda(lex);
                case TokenKind::DEFINE:
                    return parseDefine(lex);
                case TokenKind::TLAMBDA:
                    return parseTypeLambda(lex);
                case TokenKind::TAPPLY:
                    return parseTypeApplication(lex);
                case TokenKind::QUOTE:
                case TokenKind::QQUOTE:
                case TokenKind::UNQUOTE:
                case TokenKind::UNQUOTESPLICE:
                    return parseQuote(lex);
                case TokenKind::LET:
                case TokenKind::LETS:
                case TokenKind::LETR:
                    return parseLet(lex);
                default :
                    return TokenNode{parseList(lex)};
            }
        case TokenKind::QUOTE:
        case TokenKind::QQUOTE:
        case TokenKind::UNQUOTE:
        case TokenKind::UNQUOTESPLICE:
            return parseQuote(lex);
        case TokenKind::IDENT:
            return unwrapIdent(lex);
        default:
            throw std::runtime_error("error" + toString(lex.peek(0)));
    }
}

