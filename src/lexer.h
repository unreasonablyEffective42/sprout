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
void skipComment(Lexer& lex); 

Token lexNumber(Lexer& lex);
Token lexParen(Lexer& lex);
Token lexSymbol(Lexer& lex);
Token lexString(Lexer& lex);
Token lexArrow(Lexer& lex);
Token lexColon(Lexer& lex);
Token lexBool(Lexer& lex);
Token lexDot(Lexer& lex);
Token lexQuote(Lexer& lex);


struct Lexer {
    inline static Token eof = Token(TokenKind::END,0,0);
    std::string src;
    int pos = 0, column = 0, line = 0;
    int size;
 
    std::deque<Token> buffer;
    std::size_t index = 0;
    
    Lexer(std::string src_);

    Token advance();
    const Token& peek(std::size_t lookahead);
    Token next();
    void backup();
    void ensure(std::size_t i);
};

#endif
