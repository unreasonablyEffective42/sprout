#ifndef SPROUT_LANG_TOKEN_H
#define SPROUT_LANG_TOKEN_H

#include "cell.h"
#include "value.h"
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <variant>

/*
 * Token struct and enum class of token types(TokenKind) to be emitted by the
 * Lexical Analyzer and parsed into the CST(concrete syntax tree) in the parser
 */

// The different kinds of tokens
enum class TokenKind {
    END,
    NUMBER,
    IDENT,
    BOOL,
    SYMBOL,
    CHAR,
    STRING,
    LAMBDA,
    COND,
    CLAUSE,
    QUOTE,
    QQUOTE,
    UNQUOTE,
    UNQUOTESPLICE,
    DEFINE,
    LIST,
    CONS,
    LET,
    LETS,
    LETR,
    LET_BINDING,
    LPAREN,
    RPAREN,
    COLON,
    ARROW,
    DOT,
    TYPE_IDENT,
    TYPE_LIST,
    VAR_TYPE,
    TYPE_VAR,
    PARAM_LIST,
    TYPE_PARAM_LIST,
    RETURN_TYPE,
    JUST,
    NOTHING,
    MAYBE,
    FORALL,
    PLACEHOLDER,
    TLAMBDA,
    TAPPLY,
    PERFORM,
    HANDLE,
    RETURN,
    ERROR,
    RAISE,
    TRY,
    CATCH,
    DATA,
    CTOR_DECL,
    MATCH,
    PATTERN,
    PATTERN_CLAUSE,
    EQ,
    EQUALS,
    SHIFT,
    RESET,
    FORCE,
    DO,
    NIL
};

// All tokens have a token kind, but hold an optional value as not
// all tokenkinds need a value, additionally the line and column are
// recorded by the Lexer for debug information and error messaging
struct Token {
    TokenKind kind = TokenKind::NIL;
    std::optional<Value> value = std::nullopt;
    int line = 0;
    int column = 0;

    Token();
    Token(TokenKind kind_, int line_, int column_);
    Token(TokenKind kind_, Value value_, int line_, int column_);
};

// TokenList is a cons-list whose elements are TokenNode.
// TokenNode is either a Token (atom) or a nested TokenList.
struct TokenListNode;
using TokenList = std::shared_ptr<const TokenListNode>;
using TokenNode = std::variant<Token, TokenList>;

struct TokenListNode : Cell<TokenNode, TokenList> {
    TokenListNode(TokenNode a, TokenList d)
        : Cell<TokenNode, TokenList>(std::move(a), std::move(d)) {}
};

// Lisp style cons for forming tree structures out of token nodes
inline TokenList cons(TokenNode a, TokenList d) {
    return std::make_shared<TokenListNode>(std::move(a), std::move(d));
}

// head returns the token node at the head of the tokenList,
// while tail returns the rest of the tokenlist
inline const TokenNode &head(const TokenList &lst) { return lst->car; }
inline const TokenList &tail(const TokenList &lst) { return lst->cdr; }

std::ostream &operator<<(std::ostream &os, const TokenList &lst);
std::ostream &operator<<(std::ostream &os, const TokenNode &node);
std::size_t size(const TokenNode &node);
std::size_t size(const TokenList &lst);

bool operator==(const Token &a, const Token &b);
std::ostream &operator<<(std::ostream &os, const Token &tok);

std::string toString(TokenKind k);

inline std::string toString(const Token &tok) {
    std::ostringstream oss;
    oss << tok;
    return oss.str();
}

// token type checks, will probably remove as the type checks are all done in a
// direct style currently in the lexer and parser
inline bool isEnd(const Token &tok) { return tok.kind == TokenKind::END; }
inline bool isNumber(const Token &tok) { return tok.kind == TokenKind::NUMBER; }
inline bool isIdent(const Token &tok) { return tok.kind == TokenKind::IDENT; }
inline bool isSymbol(const Token &tok) { return tok.kind == TokenKind::SYMBOL; }
inline bool isBool(const Token &tok) { return tok.kind == TokenKind::BOOL; }
inline bool isChar(const Token &tok) { return tok.kind == TokenKind::CHAR; }
inline bool isString(const Token &tok) { return tok.kind == TokenKind::STRING; }
inline bool isLambda(const Token &tok) { return tok.kind == TokenKind::LAMBDA; }
inline bool isCond(const Token &tok) { return tok.kind == TokenKind::COND; }
inline bool isClause(const Token &tok) { return tok.kind == TokenKind::CLAUSE; }
inline bool isQuote(const Token &tok) { return tok.kind == TokenKind::QUOTE; }
inline bool isQQuote(const Token &tok) { return tok.kind == TokenKind::QQUOTE; }
inline bool isUnquote(const Token &tok) {
    return tok.kind == TokenKind::UNQUOTE;
}
inline bool isUnquoteSplice(const Token &tok) {
    return tok.kind == TokenKind::UNQUOTESPLICE;
}
inline bool isDefine(const Token &tok) { return tok.kind == TokenKind::DEFINE; }
inline bool isList(const Token &tok) { return tok.kind == TokenKind::LIST; }
inline bool isCons(const Token &tok) { return tok.kind == TokenKind::CONS; }
inline bool isLet(const Token &tok) { return tok.kind == TokenKind::LET; }
inline bool isLetS(const Token &tok) { return tok.kind == TokenKind::LETS; }
inline bool isLetR(const Token &tok) { return tok.kind == TokenKind::LETR; }
inline bool isLetBinding(const Token &tok) {
    return tok.kind == TokenKind::LET_BINDING;
}
inline bool isLParen(const Token &tok) { return tok.kind == TokenKind::LPAREN; }
inline bool isRParen(const Token &tok) { return tok.kind == TokenKind::RPAREN; }
inline bool isColon(const Token &tok) { return tok.kind == TokenKind::COLON; }
inline bool isArrow(const Token &tok) { return tok.kind == TokenKind::ARROW; }
inline bool isDot(const Token &tok) { return tok.kind == TokenKind::DOT; }
inline bool isTypeIdent(const Token &tok) {
    return tok.kind == TokenKind::TYPE_IDENT;
}
inline bool isTypeList(const Token &tok) {
    return tok.kind == TokenKind::TYPE_LIST;
}
inline bool isVarType(const Token &tok) {
    return tok.kind == TokenKind::VAR_TYPE;
}
inline bool isTypeVar(const Token &tok) {
    return tok.kind == TokenKind::TYPE_VAR;
}
inline bool isParamList(const Token &tok) {
    return tok.kind == TokenKind::PARAM_LIST;
}
inline bool isReturnType(const Token &tok) {
    return tok.kind == TokenKind::RETURN_TYPE;
}
inline bool isJust(const Token &tok) { return tok.kind == TokenKind::JUST; }
inline bool isNothing(const Token &tok) {
    return tok.kind == TokenKind::NOTHING;
}
inline bool isMaybe(const Token &tok) { return tok.kind == TokenKind::MAYBE; }
inline bool isForall(const Token &tok) { return tok.kind == TokenKind::FORALL; }
inline bool isPlaceholder(const Token &tok) {
    return tok.kind == TokenKind::PLACEHOLDER;
}
inline bool isTLambda(const Token &tok) {
    return tok.kind == TokenKind::TLAMBDA;
}
inline bool isTApply(const Token &tok) { return tok.kind == TokenKind::TAPPLY; }
inline bool isPerform(const Token &tok) {
    return tok.kind == TokenKind::PERFORM;
}
inline bool isHandle(const Token &tok) { return tok.kind == TokenKind::HANDLE; }
inline bool isReturn(const Token &tok) { return tok.kind == TokenKind::RETURN; }
inline bool isError(const Token &tok) { return tok.kind == TokenKind::ERROR; }
inline bool isRaise(const Token &tok) { return tok.kind == TokenKind::RAISE; }
inline bool isTry(const Token &tok) { return tok.kind == TokenKind::TRY; }
inline bool isCatch(const Token &tok) { return tok.kind == TokenKind::CATCH; }
inline bool isData(const Token &tok) { return tok.kind == TokenKind::DATA; }
inline bool isCtorDecl(const Token &tok) {
    return tok.kind == TokenKind::CTOR_DECL;
}
inline bool isMatch(const Token &tok) { return tok.kind == TokenKind::MATCH; }
inline bool isPattern(const Token &tok) {
    return tok.kind == TokenKind::PATTERN;
}
inline bool isPatternClause(const Token &tok) {
    return tok.kind == TokenKind::PATTERN_CLAUSE;
}
inline bool isEq(const Token &tok) { return tok.kind == TokenKind::EQ; }
inline bool isEquals(const Token &tok) { return tok.kind == TokenKind::EQUALS; }
inline bool isShift(const Token &tok) { return tok.kind == TokenKind::SHIFT; }
inline bool isReset(const Token &tok) { return tok.kind == TokenKind::RESET; }
inline bool isForce(const Token &tok) { return tok.kind == TokenKind::FORCE; }
inline bool isDo(const Token &tok) { return tok.kind == TokenKind::DO; }
inline bool isNil(const Token &tok) { return tok.kind == TokenKind::NIL; }
inline bool isTokenList(const TokenList &lst) { return static_cast<bool>(lst); }
inline bool isTypeParamList(const Token &tok) {
    return tok.kind == TokenKind::TYPE_PARAM_LIST;
}

inline bool isTokenNodeList(const TokenNode &node) {
    return std::holds_alternative<TokenList>(node);
}

inline bool isTokenNodeToken(const TokenNode &node) {
    return std::holds_alternative<Token>(node);
}

// unwraps the tokenNode to the inner tokenlist
inline const TokenList &asTokenList(const TokenNode &node) {
    if (!isTokenNodeList(node)) {
        throw std::runtime_error("expected TokenList node");
    }
    return std::get<TokenList>(node);
}

// custom iterator over token lists for use in the finite state machine
// validators in the parser
struct TokenListIterator {
    using value_type = const TokenNode;
    using reference = const TokenNode &;
    using pointer = const TokenNode *;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    TokenList cur;

    explicit TokenListIterator(TokenList lst) : cur(std::move(lst)) {}

    reference operator*() const { return head(cur); }
    pointer operator->() const { return &head(cur); }

    TokenListIterator &operator++() {
        cur = tail(cur);
        return *this;
    }

    TokenListIterator operator++(int) {
        TokenListIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    friend bool operator==(const TokenListIterator &a,
                           const TokenListIterator &b) {
        return a.cur == b.cur;
    }
    friend bool operator!=(const TokenListIterator &a,
                           const TokenListIterator &b) {
        return !(a == b);
    }
};

struct TokenListRange {
    TokenList lst;
    explicit TokenListRange(TokenList l) : lst(std::move(l)) {}
    TokenListIterator begin() const { return TokenListIterator(lst); }
    TokenListIterator end() const { return TokenListIterator(TokenList{}); }
};

#endif
