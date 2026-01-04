#ifndef SPROUT_LANG_TOKEN_H
#define SPROUT_LANG_TOKEN_H
 
#include "value.h"
#include <iostream>
#include <optional>

enum class TokenKind {
  END,   
  NUMBER,
  SYMBOL,
  BOOLEAN,
  CHARACTER,
  STRING,
  LAMBDA,
  COND,
  QUOTE,
  QQUOTE,
  UNQUOTE,
  UNQUOTESPLICE,
  DEFINE,
  LIST,
  LETS,
  LETR,
  LPAREN,
  RPAREN,
  DOT,
  TYPE
};

struct Token {
    TokenKind kind;
    std::optional<Value> value = std::nullopt;
    
    Token(TokenKind kind_);
    Token(TokenKind kind_, Value value_);
};

std::ostream& operator<<(std::ostream& os, const Token& tok);

#endif
