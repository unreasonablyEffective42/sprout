#ifndef SPROUT_LANG_LEXER_H
#define SPROUT_LANG_LEXER_H

#include "value.h"
#include "token.h"
#include "rational.h"
#include "complex.h"

#include <iostream>
#include <string>
#include <regex>

struct Lexer;

Value parseNumber(const std::string& candidate);

void skipWhitespace(Lexer& lex);

Token lexNumber(Lexer& lex);
Token lexParen(Lexer& lex);
Token lexSymbol(Lexer& lex);
Token lexString(Lexer& lex);
Token lexArrow(Lexer& lex);
Token lexColon(Lexer& lex);
Token lexBool(Lexer& lex);
Token lexDot(Lexer& lex);


struct Lexer {
    inline static Token eof = Token(TokenKind::END,0,0);
    std::string src;

    int pos = 0;
    int column = 0;
    int line = 0;
    int size;
 
    Token current;
    Token previous;
    
    Lexer(std::string src_);

    void advance();
    const Token& peek();
    const Token& next();
};

#endif
