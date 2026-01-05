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

#endif
