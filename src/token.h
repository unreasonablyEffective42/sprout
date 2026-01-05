#ifndef SPROUT_LANG_TOKEN_H
#define SPROUT_LANG_TOKEN_H
 
#include "value.h"
#include <iostream>
#include <optional>

enum class TokenKind {
  END,   
  NUMBER,
  IDENT,
  BOOL,
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
  LPAREN,
  RPAREN,
  COLON,
  ARROW,
  DOT,
  TYPE_IDENT,
  VAR_TYPE,
  RETURN_TYPE,
  SHIFT,
  RESET,
  FORCE,
  DO,
  NIL
};

struct Token {
    TokenKind kind = TokenKind::NIL;
    std::optional<Value> value = std::nullopt;
    int line = 0;
    int column =0;
    
    Token();
    Token(TokenKind kind_, int line_, int column_);
    Token(TokenKind kind_, Value value_, int line_, int column_);

};

bool operator==(const Token& a, const Token& b);
std::ostream& operator<<(std::ostream& os, const Token& tok);

inline bool isEnd(const Token& tok) { return tok.kind == TokenKind::END; }
inline bool isNumber(const Token& tok) { return tok.kind == TokenKind::NUMBER; }
inline bool isIdent(const Token& tok) { return tok.kind == TokenKind::IDENT; }
inline bool isBool(const Token& tok) { return tok.kind == TokenKind::BOOL; }
inline bool isChar(const Token& tok) { return tok.kind == TokenKind::CHAR; }
inline bool isString(const Token& tok) { return tok.kind == TokenKind::STRING; }
inline bool isLambda(const Token& tok) { return tok.kind == TokenKind::LAMBDA; }
inline bool isCond(const Token& tok) { return tok.kind == TokenKind::COND; }
inline bool isClause(const Token& tok) { return tok.kind == TokenKind::CLAUSE; }
inline bool isQuote(const Token& tok) { return tok.kind == TokenKind::QUOTE; }
inline bool isQQuote(const Token& tok) { return tok.kind == TokenKind::QQUOTE; }
inline bool isUnquote(const Token& tok) { return tok.kind == TokenKind::UNQUOTE; }
inline bool isUnquoteSplice(const Token& tok) { return tok.kind == TokenKind::UNQUOTESPLICE; }
inline bool isDefine(const Token& tok) { return tok.kind == TokenKind::DEFINE; }
inline bool isList(const Token& tok) { return tok.kind == TokenKind::LIST; }
inline bool isCons(const Token& tok) { return tok.kind == TokenKind::CONS; }
inline bool isLet(const Token& tok) { return tok.kind == TokenKind::LET; }
inline bool isLetS(const Token& tok) { return tok.kind == TokenKind::LETS; }
inline bool isLetR(const Token& tok) { return tok.kind == TokenKind::LETR; }
inline bool isLParen(const Token& tok) { return tok.kind == TokenKind::LPAREN; }
inline bool isRParen(const Token& tok) { return tok.kind == TokenKind::RPAREN; }
inline bool isColon(const Token& tok) { return tok.kind == TokenKind::COLON; }
inline bool isArrow(const Token& tok) { return tok.kind == TokenKind::ARROW; }
inline bool isDot(const Token& tok) { return tok.kind == TokenKind::DOT; }
inline bool isTypeIdent(const Token& tok) { return tok.kind == TokenKind::TYPE_IDENT; }
inline bool isVarType(const Token& tok) { return tok.kind == TokenKind::VAR_TYPE; }
inline bool isReturnType(const Token& tok) { return tok.kind == TokenKind::RETURN_TYPE; }
inline bool isShift(const Token& tok) { return tok.kind == TokenKind::SHIFT; }
inline bool isReset(const Token& tok) { return tok.kind == TokenKind::RESET; }
inline bool isForce(const Token& tok) { return tok.kind == TokenKind::FORCE; }
inline bool isDo(const Token& tok) { return tok.kind == TokenKind::DO; }
inline bool isNil(const Token& tok) { return tok.kind == TokenKind::NIL; }

#endif
