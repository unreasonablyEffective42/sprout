#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "value.h"
#include <sstream>
#include <stack>
#include <stdexcept>

/*
 * the sprout parser is a recursive descent parser that operates on a Lexer
 * struct, the initial call to parse(lex) then recursively dispatches to the
 * various helper parser functions based on the token kinds emitted by the
 * lexer. the final result is the TokenNode representing the concrete syntax
 * tree. if parse succeeds and returns a tree, then the source program is
 * syntactically correct from a structural standpoint, but has not undergone
 * typechecking in addition to the recursively dispatched parser helpers, there
 * are a number of validator helpers that use finite state machines to ensure
 * that a sequence of tokens matches a given pattern
 */

// called to parse a list literal, or anything list shaped that is not headed by
// a reserved word
TokenList parseList(Lexer &lex) { // called after consuming a LPAREN
    std::stack<TokenNode> elems;
    Token err = lex.peek(0);
    while (lex.peek(0).kind != TokenKind::RPAREN &&
           lex.peek(0).kind !=
               TokenKind::END) { // check if the current paren is an RPAREN
        elems.push(parse(lex));
    }
    if (lex.peek(0).kind == TokenKind::END) {
        throw std::runtime_error(
            "unterminated list found at line:" + std::to_string(err.line) +
            " column:" + std::to_string(err.column));
    }
    (void)lex.next();
    if (elems.empty()) {
        return TokenList{};
    }
    TokenList list = cons(elems.top(), TokenList{});
    elems.pop();
    while (!elems.empty()) {
        list = cons(elems.top(), list);
        elems.pop();
    }
    return list;
}

// takes a token or TokenKind::IDENT and if the value is of a reserved keyword,
// it constructs a new token of that keyword kind, and then swaps it to the
// current token in the lexer using swapcurrent if the token's .value is not of
// one of those reserved kinds, it promotes it to a symbol this does not
// interfere with IDENT also being used for type declarations as this branch is
// only entered by lists headed by tokens of these kinds
void promoteIdent(Lexer &lex) {
    const Token tok = lex.peek(0);
    if (tok.value && std::holds_alternative<Symbol>(tok.value->v)) {
        const auto &sym = std::get<Symbol>(tok.value->v);
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
        throw std::runtime_error("Ident with no value or non-symbol value" +
                                 toString(tok));
    }
}

// parses a conditional statement of the form (cond ((pred1 expr1) (pred2 expr2)
// ... (predn exprn))
TokenNode parseCond(Lexer &lex) {
    Token cond_ = lex.next();
    std::stack<TokenNode> clauses;
    if (lex.peek(0).kind == TokenKind::LPAREN) {
        while (lex.peek(0).kind == TokenKind::LPAREN) {
            clauses.push(parse(lex));
        }
        if (clauses.size() < 1) {
            throw std::runtime_error("cond expressions must have at least one "
                                     "clause, found at line:" +
                                     std::to_string(cond_.line) +
                                     " column:" + std::to_string(cond_.column));
        }
        TokenList cond = TokenList{};
        while (!clauses.empty()) {
            if (size(clauses.top()) != 2) {
                throw std::runtime_error(
                    "cond clauses must have two and only two "
                    "expressions, found at line:" +
                    std::to_string(cond_.line) +
                    " column:" + std::to_string(cond_.column));
            }
            TokenList clause = cons(TokenNode{Token(TokenKind::CLAUSE, 0, 0)},
                                    cons(clauses.top(), TokenList{}));
            cond = cons(TokenNode{clause}, cond);
            clauses.pop();
        }
        (void)lex.next(); // consume closing RPAREN;
        return cons(TokenNode{cond_}, cond);
    }
    throw std::runtime_error("bad cond form, clauses must be lists of two "
                             "expressions, found no list at line:" +
                             std::to_string(lex.peek(0).line) +
                             " column:" + std::to_string(lex.peek(0).column));
}

TokenNode parseLambda(Lexer &lex) {
    Token lambda_ = lex.next();
    if (lex.peek(0).kind == TokenKind::LPAREN) {
        TokenNode parameters = parse(lex);
        parameters = validateParams(parameters);
        TokenNode body = parse(lex);
        if (lex.peek(0).kind != TokenKind::RPAREN) {
            throw std::runtime_error(
                "lambda expressions may only have one body expression, found:" +
                toString(lex.peek(0)));
        }
        (void)lex.next(); // consume closing rparen
        return TokenNode{cons(TokenNode{lambda_},
                              cons(parameters, cons(body, TokenList{})))};
    }
    throw std::runtime_error(
        "lambda must be followed by parameter list, found:" +
        toString(lex.peek(0)));
}

/* horrible state machine for validating a parameter list in define and lambda
 * param lists have the form (binding1:type1 binding2:type2 ... bindingn:typen
 * -> returntype) pattern <symbol colon <type or symbol> ...> arrow <type or
 * symbol>
 */
TokenNode validateParams(const TokenNode &params) {
    int state = 0;
    const TokenList &lst = asTokenList(params);
    TokenListIterator it(lst);
    TokenListIterator end(TokenList{});
    std::stack<TokenNode> argument;
    std::stack<TokenList> arguments;
    TokenList arglist;
    while (it != end) {
        const TokenNode &node = *it;
        if (isTokenNodeList(node)) { // if TokenList and state == 3 validate the
                                     // list and push it
            switch (state) {
            case 3:
                argument.push(validateTypeList(node));
                it++;
                state = 1;
                continue;
            case 4: {
                TokenNode node_ = validateTypeList(node);
                const Token &errtok = std::get<Token>(node_);
                auto ast = std::make_shared<AstNode>(node_);
                Token ret = Token(TokenKind::RETURN_TYPE, Value(ast),
                                  errtok.line, errtok.column);
                TokenList params_ = cons(TokenNode{ret}, TokenList{});

                while (!arguments.empty()) {
                    params_ = cons(TokenNode{arguments.top()}, params_);
                    arguments.pop();
                }
                it++;
                if (it != end) {
                    throw std::runtime_error(
                        "param list did not terminate in a typelist "
                        "or has more than one in lambda at line: " +
                        toString(ret));
                }
                TokenList wrapped = cons(
                    TokenNode{Token(TokenKind::PARAM_LIST, 0, 0)}, params_);
                return TokenNode{wrapped};
            }
            default:
                std::ostringstream oss;
                oss << node;
                std::string msg =
                    (state == 0
                         ? "param list must begin with a symbol, found:"
                         : (state == 1 ? "expected argument or arrow, found:"
                                       : "expected colon, found:"));
                throw std::runtime_error(msg + oss.str());
            }
        } else {
            const Token &tok = std::get<Token>(node);
            switch (state) {
            case 0: // expect TokenKind::SYMBOL
                if (tok.kind != TokenKind::SYMBOL) {
                    throw std::runtime_error(
                        "param list must begin with a symbol, found:" +
                        toString(tok));
                }
                state = 2;
                argument.push(TokenNode{tok});
                it++;
                continue;
            case 1: // expect TokenKind::SYMBOL or TokenKind::ARROW
                arglist = TokenList{};
                while (!argument.empty()) {
                    arglist = cons(TokenNode{argument.top()}, arglist);
                    argument.pop();
                }
                arguments.push(arglist);
                switch (tok.kind) {
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
                    throw std::runtime_error(
                        "expected argument or arrow, found:" + toString(tok));
                }
                break;
            case 2: // expect TokenKind::COLON
                if (tok.kind != TokenKind::COLON) {
                    throw std::runtime_error("expected colon, found:" +
                                             toString(tok));
                }
                state++;
                it++;
                continue;
            case 3: // expect TokenKind::TYPE_IDENT or SYMBOL
                if (tok.kind == TokenKind::TYPE_IDENT) {
                    state = 1;
                    argument.push(TokenNode{tok});
                    it++;
                    continue;
                } else if (tok.kind == TokenKind::SYMBOL) {
                    state = 1;
                    argument.push(
                        TokenNode{Token(TokenKind::TYPE_VAR, *tok.value,
                                        tok.line, tok.column)});
                    it++;
                    continue;
                } else {
                    throw std::runtime_error("expected type, found:" +
                                             toString(tok));
                }
            case 4:
                if (tok.kind != TokenKind::TYPE_IDENT &&
                    tok.kind != TokenKind::SYMBOL) {
                    throw std::runtime_error("expected type, found:" +
                                             toString(tok));
                }
                Token temp = tok;
                if (tok.kind == TokenKind::SYMBOL) {
                    temp = Token(TokenKind::TYPE_VAR, *tok.value, tok.line,
                                 tok.column);
                }
                auto ast = std::make_shared<AstNode>(TokenNode{temp});
                Token ret = Token(TokenKind::RETURN_TYPE, Value(ast), tok.line,
                                  tok.column);
                TokenList params_ = cons(TokenNode{ret}, TokenList{});
                while (!arguments.empty()) {
                    params_ = cons(TokenNode{arguments.top()}, params_);
                    arguments.pop();
                }

                it++;
                if (it != end) {
                    throw std::runtime_error(
                        "param list did not terminate in a type or "
                        "has more than one in lambda at line: " +
                        std::to_string(tok.line));
                }
                TokenList wrapped = cons(
                    TokenNode{Token(TokenKind::PARAM_LIST, 0, 0)}, params_);
                return TokenNode{wrapped};
            }
        }
    }
    throw std::runtime_error(
        "invalid param list"); // idk how we get here but // okay
}
/* another state machine, validator for typelists
 * i.e. (int->int->int)                                   a function that takes in two integers and returns an integer 
 *      (bool->(rational->complex)->((Vec int 3)->int))   a function that takes in a bool and a function that 
 *                                                        iself takes a rational, and returning a complex number, 
 *                                                        that returns a function that takes in a vector of three integers and returns an integer
 */
TokenNode validateTypeList(const TokenNode &types) {
    if (isTokenNodeToken(types)) {
        const Token &type = std::get<Token>(types);
        if (type.kind == TokenKind::TYPE_IDENT && type.value.has_value() &&
            isAstPtr(*type.value)) {
            return types;
        }
        throw std::runtime_error("expected type list, found: " +
                                 toString(type));
    }
    int state = 0;
    const TokenList &lst = asTokenList(types);
    const Token &headtok = std::get<Token>(head(lst));
    if (headtok.kind == TokenKind::FORALL) {
        return validateForall(types);
    }
    TokenListIterator it(lst);
    TokenListIterator end(TokenList{});
    std::stack<TokenNode> typeNodes;
    const Token *errtok = nullptr;
    while (it != end) {
        const TokenNode &node = *it;
        switch (state) {
        case 0: // expect a type, a symbol or a TokenList
            if (isTokenNodeList(node)) {
                typeNodes.push(validateTypeList(node));
                it++;
                state = 1;
                continue;
            } else {
                const Token &tok = std::get<Token>(node);
                if (tok.kind == TokenKind::TYPE_IDENT) {
                    typeNodes.push(node);
                    it++;
                    state = 1;
                    continue;
                } else if (tok.kind == TokenKind::SYMBOL) {
                    typeNodes.push(
                        TokenNode{Token(TokenKind::TYPE_VAR, *tok.value,
                                        tok.line, tok.column)});
                    it++;
                    state = 1;
                    continue;
                } else {
                    throw std::runtime_error(
                        "expected type in type list, found: " + toString(tok));
                }
            }
            break;
        case 1: // expect an arrow, or a type/tokenlist
            if (isTokenNodeToken(node)) {
                const Token &tok = std::get<Token>(node);
                if (tok.kind == TokenKind::ARROW) {
                    it++;
                    state = 0;
                    errtok = &tok;
                    continue;
                } else if (tok.kind == TokenKind::TYPE_IDENT) {
                    typeNodes.push(node);
                    it++;
                    state = 2;
                    continue;
                } else if (tok.kind == TokenKind::SYMBOL) {
                    typeNodes.push(
                        TokenNode{Token(TokenKind::TYPE_VAR, *tok.value,
                                        tok.line, tok.column)});
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
        case 2: // expect int
            if (isTokenNodeToken(node)) {
                const Token &tok = std::get<Token>(node);
                if (tok.kind == TokenKind::NUMBER) {
                    typeNodes.push(node);
                    it++;
                    state = 1;
                    continue;
                }
                throw std::runtime_error(
                    "expected number in composite type, found:" +
                    toString(tok));
            }
            {
                std::ostringstream oss;
                oss << node;
                throw std::runtime_error(
                    "expected number in composite type, found:" + oss.str());
            }
            break;
        }
    }
    if (state == 0) {
        throw std::runtime_error(
            "typeList did not terminate in a type, found " +
            (errtok ? toString(*errtok) : std::string("<none>")));
    }

    TokenList temp = TokenList{};
    while (!typeNodes.empty()) {
        temp = cons(typeNodes.top(), temp);
        typeNodes.pop();
    }
    auto ast = std::make_shared<AstNode>(TokenNode{temp});
    return TokenNode{Token(TokenKind::TYPE_IDENT, Value(ast), 0, 0)};
}

//unwraps an IDENT token to a SYMBOL token using the value of the IDENT
TokenNode unwrapIdent(Lexer &lex) {
    Token temp = lex.next();
    if (!temp.value) {
        throw std::runtime_error(
            "attempted to unwrap identifier with no value: " + toString(temp));
    }
    const auto &sym = std::get<Symbol>(temp.value->v);
    if (sym.name == "else") {
        return TokenNode{
            Token(TokenKind::BOOL, Value(true), temp.line, temp.column)};
    }
    return TokenNode{
        Token(TokenKind::SYMBOL, Value(sym.name), temp.line, temp.column)};
}

/* Helper for define statements 
 * ex  (define a:complex 3+2i)
 *     (define foo (x:int y:int -> int) body) 
 *     (define foo (lambda (x:int y:int -> int) body))
 */
TokenNode parseDefine(Lexer &lex) {
    Token root = lex.next();
    if (lex.peek(0).kind != TokenKind::IDENT) {
        throw std::runtime_error("expected symbol after define, found:" +
                                 toString(lex.peek(0)));
    }
    const TokenNode sym = parse(lex);
    switch (lex.peek(0).kind) {
    case TokenKind::COLON: { // (define x:int expr) or (define x:(vec int 3)
                             // expr)
        (void)lex.next();
        TokenNode type = parse(lex);
        TokenNode expr = parse(lex);

        if (isTokenNodeList(type)) {
            type = validateTypeList(type);
        }
        (void)lex.next(); // consume closing rparen
        return TokenNode{cons(TokenNode{root},
                              cons(sym, cons(type, cons(expr, TokenList{}))))};
    }

    case TokenKind::LPAREN: { // (define foo (params ...) expr) or (define foo
                              // (lambda ...))
        Token temp = lex.peek(1);
        if (temp.kind == TokenKind::IDENT && temp.value &&
            std::get<Symbol>(temp.value->v).name == "lambda") {
            TokenNode lambda = parse(lex);
            (void)lex.next(); // consume closing rparen
            return TokenNode{
                cons(TokenNode{root}, cons(sym, cons(lambda, TokenList{})))};
        }
        TokenNode type = parse(lex);
        TokenNode expr = parse(lex);
        if (isTokenNodeList(type)) {
            type = validateParams(type);
            auto ast = std::make_shared<AstNode>(type);
            type = TokenNode{Token(TokenKind::TYPE_IDENT, Value(ast), 0, 0)};
        }
        (void)lex.next(); // consume closing rparen
        return TokenNode{cons(TokenNode{root},
                              cons(sym, cons(type, cons(expr, TokenList{}))))};
    }
    default:
        throw std::runtime_error("expected type or closure, found:" +
                                 toString(lex.peek(0)));
    }
}

/* validateQuote and validateQuoteList work to ensure proper quotation syntax
 * recursively validates a quoted datum so that unquotes only happen if there are sufficient quasiquotes
 * in the layers above
 * ex '(,x 1) is invalid as it is just quoted, qquote depth is 0 
 *    `(,x 1) is valid as there is one qquote to match its depth
 *    `(1 ,@(2 3)) is valid as the unquote splice is also has a matching qquote
 *    `(,(foo ,x 2) 3 4) is invalid as the inner (foo ,x 2) was already unquoted, the expression is maximally unquoted 
 *    `(,(foo x 2) `(4 5 ,@(6 7))) is valid as the unquoted foo does not have an unquote at an incorrect dept, and the
 *                                 unquote-splice (,@) occurs at an even greater depth
 */
bool validateQuoteList(const TokenList &lst, int depth) {
    for (TokenListIterator it(lst), end(TokenList{}); it != end; ++it) {
        if (!validateQuote(*it, depth))
            return false;
    }
    return true;
}

bool validateQuote(const TokenNode &node, int depth) {
    if (isTokenNodeToken(node)) {
        const Token &tok = std::get<Token>(node);
        if (tok.kind == TokenKind::UNQUOTE ||
            tok.kind == TokenKind::UNQUOTESPLICE) {
            return depth > 0;
        }
        return true;
    }
    const TokenList &lst = asTokenList(node);
    if (!lst)
        return true;

    // If list begins with quote-like token, adjust depth for its argument.
    const TokenNode &headNode = head(lst);
    if (isTokenNodeToken(headNode)) {
        const Token &headTok = std::get<Token>(headNode);
        int nextDepth = depth;
        if (headTok.kind == TokenKind::QQUOTE)
            nextDepth = depth + 1;
        else if (headTok.kind == TokenKind::UNQUOTE ||
                 headTok.kind == TokenKind::UNQUOTESPLICE) {
            if (depth == 0)
                return false;
            nextDepth = depth - 1;
        }
        // validate the rest of the list at nextDepth
        return validateQuoteList(tail(lst), nextDepth);
    }
    return validateQuoteList(lst, depth);
}

/* helper for parsing quoted expressions
 * '(1 2 (foo 3))     a list literal of the list (1 2 (foo 3)
 * `(1 2 ,(foo 3))    if (foo 3) = 4 a list literal of (1 2 4), the unquote(,) evaluates the unquoted expression
 * '(1 2 (3 4))       a list literal of the nested list (1 2 (3 4)) 
 * `(1 2 ,@(3 4))     unquote splice unwraps the inner list, returning the list literal (1 2 3 4)
 */
TokenNode parseQuote(Lexer &lex) {
    Token root = lex.next();
    TokenNode quoted = parse(lex);
    int depth = (root.kind == TokenKind::QQUOTE) ? 1 : 0;
    if ((root.kind == TokenKind::UNQUOTE ||
         root.kind == TokenKind::UNQUOTESPLICE) &&
        depth == 0) {
        throw std::runtime_error("unquote outside quasiquote" + toString(root));
    }
    if (validateQuote(quoted, depth)) {
        return TokenNode{cons(TokenNode{root}, cons(quoted, TokenList{}))};
    }
    throw std::runtime_error("unreachable");
}
/*
 * helper for parsing the bingind expressions in a let/s/r expression
 *(let ((x:int 0)                                       ;first binding 
 *      (y:(int->int) (lambda (z:int -> int) expr)))    ;second binding
 *  body)
*/
TokenNode parseBinding(Lexer &lex) {
    Token lparen = lex.next();
    TokenNode sym, type, val;
    if (lex.peek(0).kind == TokenKind::IDENT) {
        sym = parse(lex);
        if (lex.peek(0).kind == TokenKind::COLON) {
            (void)lex.next();
            if (lex.peek(0).kind == TokenKind::TYPE_IDENT) {
                type = parse(lex);
            } else if (lex.peek(0).kind == TokenKind::LPAREN) {
                type = parse(lex);
                type = validateTypeList(type);
            } else if (lex.peek(0).kind == TokenKind::IDENT) {
                TokenNode t = parse(lex);
                const Token &tok = std::get<Token>(t);
                if (tok.kind == TokenKind::SYMBOL && tok.value) {
                    type = TokenNode{Token(TokenKind::TYPE_VAR, *tok.value,
                                           tok.line, tok.column)};
                } else {
                    throw std::runtime_error(
                        "expected a type var in type position, found:" +
                        toString(tok));
                }

            } else {
                throw std::runtime_error("expected type, found:" +
                                         toString(lex.peek(0)));
            }
            val = parse(lex);
            TokenNode binding =
                TokenNode{cons(sym, cons(type, cons(val, TokenList{})))};
            auto ast = std::make_shared<AstNode>(binding);
            Token root = Token(TokenKind::LET_BINDING, Value(ast), lparen.line,
                               lparen.column);
            if (lex.peek(0).kind == TokenKind::RPAREN) {
                (void)lex.next();
                return TokenNode(root);
            }
            throw std::runtime_error("expected ')' to close binding, found:" +
                                     toString(lex.peek(0)));
        }
        throw std::runtime_error("expected ':' in binding type, found:" +
                                 toString(lex.peek(0)));
    }
    throw std::runtime_error("bindings must start with a symbol, found:" +
                             toString(lex.peek(0)));
}
/* Helper for parsing let/s/r expressions 
 * ex: (let ((x:int 3)            ;bind values at once/no specified order
 *           (y:int 2)) 
 *       (+ x y))
 *     
 *     (lets ((x:int 3)           ;bind values sequentially, latter bindings can refer to earlier binginds
 *            (y:int (+ x 2))
 *            (z:int (* x y)))
 *       (/ z 5))
 *
 *     (letr ((foo:(int->int)    ;bind values with omnidirectional references and recursions
 *             (lambda (n)
 *                (bar (+ n 3))))
 *           (bar:(int->int)
 *             (lambda (n) 
 *                 (foo (- n 2)))))
 *       (foo 2))
 */
TokenNode parseLet(Lexer &lex) {
    Token root = lex.next();
    TokenNode expr, name = TokenNode{Token(TokenKind::SYMBOL, 0, 0)};
    std::stack<TokenNode> bindings;
    TokenList bindings_ = TokenList{};
    if (lex.peek(0).kind == TokenKind::IDENT) { // named let branch
        name = parse(lex);
        Token name_;
        if (isTokenNodeToken(name)) {
            name_ = std::get<Token>(name);
        } else {
            std::ostringstream oss;
            oss << name;
            throw std::runtime_error("expected a name for named let, found:" +
                                     oss.str());
        }

        if (name_.kind == TokenKind::SYMBOL) {
            if (lex.peek(0).kind == TokenKind::LPAREN) {
                (void)lex.next();
                while (lex.peek(0).kind != TokenKind::RPAREN) {
                    bindings.push(parseBinding(lex));
                }
                (void)lex.next(); // consume RPAREN
                while (!bindings.empty()) {
                    bindings_ = cons(bindings.top(), bindings_);
                    bindings.pop();
                }
            } else {
                throw std::runtime_error(
                    "named " + toString(root.kind) +
                    " not followed with bindings list, found:" +
                    toString(lex.peek(0)));
            }
        }
    } else if (lex.peek(0).kind == TokenKind::LPAREN) { // regular let branch
        (void)lex.next();
        while (lex.peek(0).kind != TokenKind::RPAREN) {
            bindings.push(parseBinding(lex));
        }
        (void)lex.next(); // consume RPAREN
        while (!bindings.empty()) {
            bindings_ = cons(bindings.top(), bindings_);
            bindings.pop();
        }
    } else {
        throw std::runtime_error(
            toString(root.kind) +
            " bindings must begin with a name or a bindings list, found:" +
            toString(lex.peek(0)));
    }
    expr = parse(lex);
    (void)lex.next(); // consume closing rparen
    return TokenNode{
        cons(TokenNode{root},
             cons(name, cons(TokenNode{bindings_}, cons(expr, TokenList{}))))};
}

//validates type parameters in system f forall type lambda expressions
TokenNode validateTypeParams(const TokenNode &params) {
    const TokenList &lst = asTokenList(params);
    TokenListIterator it(lst);
    TokenListIterator end(TokenList{});
    std::stack<Token> parameters;
    while (it != end) {
        const TokenNode &node = *it;
        if (isTokenNodeToken(node)) {
            const Token &tok = std::get<Token>(node);
            if (tok.kind == TokenKind::SYMBOL && tok.value) {
                parameters.push(Token(TokenKind::TYPE_VAR, *tok.value, tok.line,
                                      tok.column));
                it++;
                continue;
            }
            throw std::runtime_error("expected type variable in parameters "
                                     "list for type lambda, found:" +
                                     toString(tok));
        }
        std::ostringstream oss;
        oss << node;
        throw std::runtime_error(
            "expected flat parameter list in type lambda, found" + oss.str());
    }
    TokenList params_ = TokenList{};
    if (parameters.empty()) {
        throw std::runtime_error("type lambdas cannot have no parameters");
    }
    while (!parameters.empty()) {
        params_ = cons(TokenNode{parameters.top()}, params_);
        parameters.pop();
    }
    auto ast = std::make_shared<AstNode>(TokenNode{params_});
    return TokenNode{Token(TokenKind::TYPE_PARAM_LIST, Value(ast), 0, 0)};
}

/* parse system f type lambdas over polymorphic type variables
* (define id:(forall (A) (A -> A))
*   (tlambda (A)
*     (lambda (x:A -> A)
*       x)))
*/
TokenNode parseTypeLambda(Lexer &lex) {
    Token tlambda_ = lex.next();
    if (lex.peek(0).kind == TokenKind::LPAREN) {
        TokenNode parameters = parse(lex);
        parameters = validateTypeParams(parameters);
        TokenNode body = parse(lex);
        if (lex.peek(0).kind != TokenKind::RPAREN) {
            throw std::runtime_error("type lambda expressions may only have "
                                     "one body expression, found:" +
                                     toString(lex.peek(0)));
        }
        (void)lex.next();
        return TokenNode{cons(TokenNode{tlambda_},
                              cons(parameters, cons(body, TokenList{})))};
    }
    throw std::runtime_error(
        "type lambda expressions must be followed by parameter list, found:" +
        toString(lex.peek(0)));
}

/* parse type application to a tlambda 
*  ;Instantiate id at int, then apply
*  ((tapply id int) 42)
*/
TokenNode parseTypeApplication(Lexer &lex) {
    Token tapply = lex.next();
    TokenNode expr = parse(lex);

    std::stack<TokenNode> types;
    if (lex.peek(0).kind == TokenKind::RPAREN) {
        throw std::runtime_error(
            "attempted to apply type lambda to no types in:" +
            toString(tapply));
    }

    while (lex.peek(0).kind != TokenKind::RPAREN) {
        TokenNode ty = parse(lex); // handles IDENT -> SYMBOL
        if (isTokenNodeList(ty)) {
            ty = validateTypeList(ty);
        } else {
            const Token &tok = std::get<Token>(ty);
            if (tok.kind == TokenKind::SYMBOL && tok.value) {
                ty = TokenNode{Token(TokenKind::TYPE_VAR, *tok.value, tok.line,
                                     tok.column)};
            } else if (tok.kind != TokenKind::TYPE_IDENT) {
                throw std::runtime_error("expected type in tapply, found:" +
                                         toString(tok));
            }
        }
        types.push(ty);
    }

    (void)lex.next(); // consume ')'

    TokenList types_ = TokenList{};
    while (!types.empty()) {
        types_ = cons(types.top(), types_);
        types.pop();
    }

    return TokenNode{cons(TokenNode{tapply}, cons(expr, types_))};
}

//validator for forall expressions (forall (A) .....)
TokenNode validateForall(const TokenNode &forall) {
    const TokenList &lst = asTokenList(forall);
    TokenListIterator it(lst);
    TokenListIterator end(TokenList{});
    Token forall_ = std::get<Token>(*it);
    if (forall_.kind != TokenKind::FORALL) {
        throw std::runtime_error("encountered: " + toString(forall_) +
                                 " in forall");
    }
    it++;
    if (!isTokenNodeList(*it)) {
        throw std::runtime_error("expected type variable list, found:" +
                                 toString(std::get<Token>(*it)));
    }
    TokenNode params = validateTypeParams(*it);
    it++;
    if (!isTokenNodeList(*it)) {
        throw std::runtime_error("expected type list, found:" +
                                 toString(std::get<Token>(*it)));
    }
    TokenNode body = validateTypeList(*it);
    it++;
    if (it != end) {
        std::ostringstream oss;
        oss << *it;
        throw std::runtime_error("forall type expresssions may only have one "
                                 "body expression, found:" +
                                 oss.str());
    }
    auto ast = std::make_shared<AstNode>(TokenNode{
        cons(TokenNode{forall_}, cons(params, cons(body, TokenList{})))});
    return TokenNode{
        Token(TokenKind::TYPE_IDENT, Value(ast), forall_.line, forall_.column)};
}

/* validate list literals, ensure that dotted pairs and lists only have one dot and before the final element 
 *  (1 . 2) a cons pair of 1 and 2 
 *  (1 2) a proper list of 1 2 and the implicit empty list (1 2 ())
 *  (1 2 . 3) improper list literal i.e (1 . (2 . 3)) 
 *  (1 . 2 3) invalid and nonsensical construction 
*/
TokenList validateDottedList(const TokenList &lst) {
    if (!lst) {
        return lst;
    }
    std::size_t len = 0;
    std::size_t dot_pos = 0;
    std::size_t dot_count = 0;
    int line = 0;
    for (TokenListIterator it(lst), end(TokenList{}); it != end; ++it) {
        const TokenNode &node = *it;
        ++len;
        if (isTokenNodeToken(node)) {
            const Token &tok = std::get<Token>(node);
            if (len == 1) {
                line = tok.line;
            }
            if (tok.kind == TokenKind::DOT) {
                ++dot_count;
                dot_pos = len;
                if (dot_count > 1) {
                    throw std::runtime_error("list literals cannot contain "
                                             "more than one dot, at line:" +
                                             std::to_string(line));
                }
            }
        } else {
            const TokenList &sub = std::get<TokenList>(node);
            if (len == 1 && sub) {
                // Best-effort line number from nested head.
                const TokenNode &subHead = head(sub);
                if (isTokenNodeToken(subHead)) {
                    line = std::get<Token>(subHead).line;
                }
            }
            validateDottedList(sub);
        }
    }
    if (dot_count == 1) {
        if (len < 3) {
            throw std::runtime_error(
                "dotted lists cannot be of size < 3, at line:" +
                std::to_string(line));
        }
        if (dot_pos != len - 1) {
            throw std::runtime_error("dotted list did not terminate in pattern "
                                     "(... . expr), at line:" +
                                     std::to_string(line));
        }
    }
    return lst;
}

//helper for grabbing the first token in a nested structure, used for getting the line and column for error reporting
static bool firstTokenInNode(const TokenNode &node, Token &out) {
    if (isTokenNodeToken(node)) {
        out = std::get<Token>(node);
        return true;
    }
    const TokenList &lst = std::get<TokenList>(node);
    for (TokenListIterator it(lst), end(TokenList{}); it != end; ++it) {
        if (firstTokenInNode(*it, out)) {
            return true;
        }
    }
    return false;
}

/* validator for (match (pattern expr) (pattern expr))
*  in pattern matching pattern clauses 
*/
TokenNode validatePatternClause(const TokenNode &patternClause) {
    TokenNode pattern, body;
    if (!isTokenNodeList(patternClause)) {
        std::ostringstream oss;
        oss << patternClause;
        throw std::runtime_error("pattern clause must be a list, found " +
                                 oss.str());
    }
    TokenList temp = std::get<TokenList>(patternClause);
    if (size(temp) != 2) {
        throw std::runtime_error("pattern clause must have 2 elements, found " +
                                 std::to_string(size(temp)));
    }

    const TokenNode &headNode = head(temp);
    if (isTokenNodeList(headNode)) { // constructor or list pattern
        TokenList pattern_ = validateDottedList(std::get<TokenList>(headNode));
        pattern = TokenNode{pattern_};
    } else if (isTokenNodeToken(headNode)) {
        pattern = headNode;
    } else {
        throw std::runtime_error("unexpected pattern clause head");
    }

    body = head(tail(temp));

    int line = 0;
    int column = 0;
    Token locTok;
    if (firstTokenInNode(pattern, locTok)) {
        line = locTok.line;
        column = locTok.column;
    }

    auto pat = std::make_shared<AstNode>(pattern);
    pattern = TokenNode{Token(TokenKind::PATTERN, Value(pat), line, column)};
    TokenNode clause = TokenNode{cons(pattern, cons(body, TokenList{}))};
    auto ast = std::make_shared<AstNode>(clause);
    return TokenNode{
        Token(TokenKind::PATTERN_CLAUSE, Value(ast), line, column)};
}

//helper for pattern match syntax
TokenNode parseMatch(Lexer &lex) {
    const Token match = lex.next();
    TokenNode scrutinee = parse(lex);
    std::stack<TokenNode> patternClauses;
    while (lex.peek(0).kind != TokenKind::RPAREN) {
        patternClauses.push(validatePatternClause(parse(lex)));
    }
    (void)lex.next(); // consume closing rparen;
    TokenList clauses = TokenList{};
    while (!patternClauses.empty()) {
        clauses = cons(patternClauses.top(), clauses);
        patternClauses.pop();
    }
    auto pattern = std::make_shared<AstNode>(scrutinee);
    scrutinee = TokenNode{
        Token(TokenKind::PATTERN, Value(pattern), match.line, match.column)};
    return TokenNode{
        cons(TokenNode{Token(TokenKind::MATCH, match.line, match.column)},
             cons(scrutinee, cons(TokenNode{clauses}, TokenList{})))};
}

static TokenNode normalizeTypeNode(const TokenNode &node) {
    if (isTokenNodeList(node)) {
        return validateTypeList(node);
    }
    const Token &tok = std::get<Token>(node);
    if (tok.kind == TokenKind::SYMBOL && tok.value) {
        return TokenNode{
            Token(TokenKind::TYPE_VAR, *tok.value, tok.line, tok.column)};
    }
    if (tok.kind == TokenKind::TYPE_IDENT) {
        return node;
    }
    throw std::runtime_error("expected type in constructor field, found " +
                             toString(tok));
}

//validates type constructors
TokenNode validateCtorDecl(const TokenNode &ctor) {
    Token locTok;
    if (isTokenNodeList(ctor)) {
        if (!firstTokenInNode(ctor, locTok)) {
            throw std::runtime_error("unable to find a token in constructor");
        }
        TokenList lst = asTokenList(ctor);
        if (size(lst) == 1 || size(lst) == 2) {
            const TokenNode &nameNode = head(lst);
            if (!isTokenNodeToken(nameNode)) {
                throw std::runtime_error(
                    "constructor name must be an identifier");
            }
            const Token &nameTok = std::get<Token>(nameNode);
            if (nameTok.kind != TokenKind::SYMBOL) {
                throw std::runtime_error(
                    "constructor name must be an identifier, found " +
                    toString(nameTok));
            }

            if (size(lst) == 1) {
                TokenList ctorList = cons(nameNode, TokenList{});
                auto decl = std::make_shared<AstNode>(TokenNode{ctorList});
                return TokenNode{Token(TokenKind::CTOR_DECL, Value(decl),
                                       locTok.line, locTok.column)};
            }

            const TokenNode &fieldsNode = head(tail(lst));
            if (!isTokenNodeList(fieldsNode)) {
                throw std::runtime_error("constructor fields must be a list");
            }
            TokenList fields = std::get<TokenList>(fieldsNode);
            std::stack<TokenNode> normalized;
            for (TokenListIterator it(fields), end(TokenList{}); it != end;
                 ++it) {
                normalized.push(normalizeTypeNode(*it));
            }
            TokenList fieldList = TokenList{};
            while (!normalized.empty()) {
                fieldList = cons(normalized.top(), fieldList);
                normalized.pop();
            }

            TokenList ctorList =
                cons(nameNode, cons(TokenNode{fieldList}, TokenList{}));
            auto decl = std::make_shared<AstNode>(TokenNode{ctorList});
            return TokenNode{Token(TokenKind::CTOR_DECL, Value(decl),
                                   locTok.line, locTok.column)};
        }
        throw std::runtime_error(
            "constructor pattern list was of wrong arity:" +
            std::to_string(size(lst)));
    } else {
        throw std::runtime_error("expected list in sumtype declaration, found" +
                                 toString(std::get<Token>(ctor)));
    }
}

//parser for algebraic data types (ADTs)
/* (data maybe 
    * (just n)
    * (nothing))
*/
TokenNode parseADT(Lexer &lex) {
    Token data = lex.next();
    TokenNode products, typeName;
    std::stack<TokenNode> sums;
    if (lex.peek(0).kind == TokenKind::IDENT) {
        typeName = parse(lex);
    } else {
        throw std::runtime_error(
            "expected an identifier in ADT declaration, found" +
            toString(lex.peek(0)));
    }
    if (lex.peek(0).kind == TokenKind::LPAREN) {
        products = validateTypeParams(parse(lex));
    } else {
        throw std::runtime_error(
            "expected list of product types in ADT declaration, found" +
            toString(lex.peek(0)));
    }
    while (lex.peek(0).kind != TokenKind::RPAREN) {
        if (lex.peek(0).kind == TokenKind::LPAREN) {
            sums.push(validateCtorDecl(parse(lex)));
        } else {
            throw std::runtime_error(
                "expected constructor pattern in ADT declaration body, found:" +
                toString(lex.peek(0)));
        }
    }
    (void)lex.next(); // discard RPAREN
    TokenList constructors = TokenList{};
    while (!sums.empty()) {
        constructors = cons(sums.top(), constructors);
        sums.pop();
    }
    return TokenNode{
        cons(TokenNode{data}, cons(typeName, cons(products, constructors)))};
}

//the main dispatcher for parsing, every helper takes the lexer passed to parse() and performs its operation, usually itsef
//calling parse(lex) on the lexer it was itself passed after it has modified the lexers state
TokenNode parse(Lexer &lex) {
    switch (lex.peek(0).kind) {
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
    case TokenKind::PLACEHOLDER:
    case TokenKind::CONS:
        return TokenNode{lex.next()};
    case TokenKind::LPAREN:
        (void)lex.next(); // consume LPAREN
        if (lex.peek(0).kind == TokenKind::IDENT) {
            promoteIdent(lex);
        }
        switch (lex.peek(0).kind) {
        case TokenKind::COND:
            return parseCond(lex);
        case TokenKind::LAMBDA:
            return parseLambda(lex);
        case TokenKind::DEFINE:
            return parseDefine(lex);
        case TokenKind::TLAMBDA:
            return parseTypeLambda(lex);
        case TokenKind::MATCH:
            return parseMatch(lex);
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
        case TokenKind::DATA:
            return parseADT(lex);
        default:
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
