#include "value.h"
#include "token.h"
#include "rational.h"
#include "complex.h"
#include "lexer.h"

#include <iostream>
#include <string>
#include <regex>
#include <utility>
#include <stdexcept>
#include <unordered_set>

// Integer (decimal, no leading zeros except "0")
static const std::regex RE_INT(
  R"(^[+-]?(?:0|[1-9][0-9]*)$)"
);

// Float (decimal, MUST be like 1.0 ; no "1." and no ".5")
static const std::regex RE_FLOAT(
  R"(^[+-]?(?:[0-9]+\.[0-9]+)$)"
);

// Rational (integer / integer)
static const std::regex RE_RATIONAL(
  R"(^[+-]?(?:0|[1-9][0-9]*)/(?:0|[1-9][0-9]*)$)"
);

// Unsigned “double coefficient” (int or strict-float), sign handled outside
static const std::string DOUBLE_COEFF =
  R"((?:(?:0|[1-9][0-9]*)|(?:[0-9]+\.[0-9]+)))";

// Complex: a±bi OR pure imaginary ±bi / ±i / i
// Examples accepted: 3+4i, -2.0-7i, 3+i, i, -i, 4i, 0.5i
// Examples rejected: 3+0.5/2 i (rational), .5i, 1.i, 1.+2i
static const std::regex RE_COMPLEX(
  ("^(?:"
    "[+-]?" + DOUBLE_COEFF +          // a
    "[+-](?:" + DOUBLE_COEFF + ")?i"  // ± b i   (b optional "3+i")
   "|"
    "[+-]?(?:" + DOUBLE_COEFF + ")?i" // pure imaginary (coeff optional => "i")
  ")$")
);

static const std::string numsyms = "+-/i.";

Value parseNumber(const std::string& candidate) {
    if (std::regex_match(candidate, RE_COMPLEX)) { 
        return Value(cFromString(candidate));
    } else if (std::regex_match(candidate, RE_RATIONAL)) {
        return Value(rFromString(candidate));
    } else if (std::regex_match(candidate, RE_FLOAT)) {
        return Value(std::stod(candidate));
    } else if (std::regex_match(candidate, RE_INT)) {
        return Value(std::stoi(candidate));
    } else {
        throw std::runtime_error("invalid number candidate" + candidate);
    }
}

void skipWhitespace(Lexer& lex) {
    while(lex.pos < lex.size && isspace(lex.src[lex.pos])){
        if(lex.src[lex.pos] == '\n') {
            lex.line++;
            lex.column = 0;
        }
        lex.pos++;
        lex.column++;
    }
}

Token lexNumber(Lexer& lex) {
    int startColumn = lex.column;
    std::string parse = "";
    while(lex.pos < lex.size && (isdigit(lex.src[lex.pos]) || numsyms.contains(lex.src[lex.pos]))){
        parse+=lex.src[lex.pos];
        lex.pos++;
        lex.column++;
    } 
    return {TokenKind::NUMBER, parseNumber(parse), lex.line, startColumn};
}

Token lexParen(Lexer& lex) {
    int startColumn = lex.column;
    switch(lex.src[lex.pos]){
        case '(':
            lex.pos++;
            lex.column++;
            return {TokenKind::LPAREN, lex.line, startColumn};
        case ')':
            lex.pos++;
            lex.column++;
            return {TokenKind::RPAREN, lex.line, startColumn};
        default:
            throw std::runtime_error("lexParen callen on non paren character: " + std::string(1,lex.src[lex.pos]));
    }
}

Token lexSymbol(Lexer& lex) {
    int startColumn = lex.column;
    std::string parse = "";
    while(lex.pos < lex.size && (isdigit(lex.src[lex.pos]) || isalpha(lex.src[lex.pos]))){
        parse+=lex.src[lex.pos];
        lex.pos++;
        lex.column++;
    }
    static const std::unordered_set<std::string> type_idents = {
        "int",
        "rational",
        "float",
        "complex",
        "bool",
        "char",
        "string",
        "symbol",
        "list",
        "vec"
    };
    if (type_idents.contains(parse)) {
        return {TokenKind::TYPE_IDENT, Value(parse), lex.line, startColumn};
    }
    return {TokenKind::IDENT, Value(Symbol(parse)), lex.line, startColumn};
}

Token lexString(Lexer& lex) {
    int startColumn = lex.column;
    std::string parse = "";
    if (lex.pos < lex.size && lex.src[lex.pos] == '"') {
        lex.pos++;
        lex.column++;
        while(lex.pos < lex.size && lex.src[lex.pos] != '"') {
            parse+=lex.src[lex.pos];
            lex.pos++;
            lex.column++;
        }
        if (lex.pos >= lex.size) {
            throw std::runtime_error("unterminated string literal");
        }
        lex.pos++;
        lex.column++;
        if (parse.size() == 1 ) { return {TokenKind::CHAR, Value(parse[0]), lex.line, startColumn}; };
        return {TokenKind::STRING, Value(parse), lex.line, startColumn};
    } else {
        throw std::runtime_error("lexString called when the current char was not a \"");
    }
}

Token lexArrow(Lexer& lex) {
    int startColumn = lex.column;
    if (lex.pos + 1 < lex.size) {
        lex.pos    += 2;
        lex.column += 2;
        return {TokenKind::ARROW, lex.line, startColumn};
    }
    throw std::runtime_error("unterminated arrow token");
}

Lexer::Lexer(std::string src_){
    this->src = src_;
    this->size = src.size();
    this->advance();
}

Token lexColon(Lexer& lex) {
    int startColumn = lex.column;
    if (lex.pos < lex.size){
        lex.pos++;
        lex.column++;
    } 
    return {TokenKind::COLON, lex.line, startColumn};
}

Token lexBool(Lexer& lex) {
    int startColumn = lex.column;
    if (lex.pos+1 < lex.size && lex.src[lex.pos] == '#') {
        char b = lex.src[lex.pos+1];
        lex.pos    += 2;
        lex.column += 2;
        switch(b) {
            case 'f':
                return {TokenKind::BOOL, Value(false), lex.line, startColumn};
            case 't':
                return {TokenKind::BOOL, Value(true), lex.line, startColumn};
            default:
                throw std::runtime_error("invalid boolean construction :#" + std::string(1,b));
        }
    }
    throw std::runtime_error{"lexBool called when the current char was not # :" + std::string(1,lex.src[lex.pos])};
}

Token lexDot(Lexer& lex) {
    int startColumn = lex.column;
    if (lex.pos < lex.size) {
        lex.pos++;
        lex.column++;
        return{TokenKind::DOT, lex.line, startColumn};
    }
    throw std::runtime_error{"lexDot called after end of source"};
}

void Lexer::advance(){
    Token temp;
    char cur, nxt;
    if (pos >= size) {
        this->current = eof;
        return;
    }
    if (pos < size){
        while(true) {
            if (pos >= size) {
                this->current = eof;
                return;
            }
            nxt = '\0';                                  //reset nxt to end of string
            cur = this->src[this->pos];                  //set current 
            if (pos < size-1) { nxt = this->src[pos+1]; } //set nxt if there is a next char
            if (isspace(cur)){
                skipWhitespace(*this);
                continue;
            } else if (isdigit(cur) || (cur == 'i' && nxt != 'n') || (nxt != '\0' && cur == '-' && (isdigit(nxt) || nxt == 'i'))) {
                temp = lexNumber(*this); 
                this->previous = std::move(current);
                this->current = std::move(temp);
                break;
            } else if (cur == '(' || cur == ')') {
                temp = lexParen(*this);
                this->previous = std::move(current);
                this->current = std::move(temp);
                break; 
            } else if (isalpha(cur)) {
                temp = lexSymbol(*this); 
                this->previous = std::move(current);
                this->current = std::move(temp);
                break;  
            } else if (cur == '"') {
                temp = lexString(*this); 
                this->previous = std::move(current);
                this->current = std::move(temp);
                break;  
            } else if (cur == '-' && nxt == '>') {
                temp = lexArrow(*this);
                this->previous = std::move(current);
                this->current = std::move(temp);
                break;   
            } else if (cur == ':') {
                temp = lexColon(*this);
                this->previous = std::move(current);
                this->current = std::move(temp);
                break;   
            } else if (cur == '.') {
                temp = lexDot(*this);
                this->previous = std::move(current);
                this->current = std::move(temp);
                break;   
            } else if (cur == '#') {
                temp = lexBool(*this);
                this->previous = std::move(current);
                this->current = std::move(temp);
                break;   
            } else {
                //TODO
                throw std::runtime_error{"unable to lex character: " + cur};
            }
        }
    } else if(this->current != eof) {
        this->current = eof;
    } else {
        ; //do nothing
    }
}

const Token& Lexer::peek() {
    return this->current;
}

const Token& Lexer::next() {
    this->advance();
    return this->current;
}
