#ifndef SPROUT_LANG_LEXER_H
#define SPROUT_LANG_LEXER_H

#include "complex.h"
#include "rational.h"
#include "token.h"
#include "value.h"

#include <deque>
#include <iostream>
#include <regex>
#include <string>

struct Lexer;
// forward declarations of the lexer helper functions,
// advance() calls the correct helpers based on the current char at pos
// parseNumber is the only parsing step handled by the lexer, taking numeric
// string literals and using regex, matches onto the correct type and constructs
// a Value variant of that number type
Value parseNumber(const std::string &candidate);
void skipWhitespace(Lexer &lex);
void skipComment(Lexer &lex);
Token lexNumber(Lexer &lex);
Token lexParen(Lexer &lex);
Token lexSymbol(Lexer &lex);
Token lexString(Lexer &lex);
Token lexArrow(Lexer &lex);
Token lexColon(Lexer &lex);
Token lexBool(Lexer &lex);
Token lexDot(Lexer &lex);
Token lexPlaceholder(Lexer &lex);
Token lexQuote(Lexer &lex);

/*
 * The lexer(Lexical analyzer) takes in a raw source as a string
 * and emits Tokens using .next(), while backup is implemented,
 * it is unneded by the parser because of the lookahead capability.
 * peek(int) allows peeking of the current token peek(0), while peek(n)
 * will look ahead to the nth token ahead if it exists, and fill the buffer
 * with all the tokens between the peeked token and the current token.
 * the lexer additionally holds the current location in the source, and
 * adds that information into the tokens it emits.
 * the lexer only emits the tokenkinds that can be identified without contex
 * other token kinds are promoted and unwrapped in the parser
 */
struct Lexer {
    inline static Token eof = Token(TokenKind::END, 0, 0);
    std::string src;
    int pos = 0, column = 0, line = 0;
    int size;

    Token current = eof;
    Token previous = eof;
    bool has_previous = false;
    std::deque<Token> buffer;

    Lexer(std::string src_);

    Token advance();
    void swapCurrent(Token t);
    const Token &peek(std::size_t lookahead = 0);
    Token next();
    void backup();
    void ensure(std::size_t i);
};

#endif
