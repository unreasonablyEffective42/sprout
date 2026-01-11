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

static const std::string numsyms     = "+-/i.";
static const std::string opsyms      = "+-*/^%$!&|=<>";
static const std::string quoteStarts = "'`,";
static const std::string arrow       = " ->";


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
            lex.pos++;
            continue;
        }
        lex.pos++;
        lex.column++;
    }
}

void skipComment(Lexer& lex) {
    if (lex.src[lex.pos] == ';') {
        while(lex.pos < lex.size) {
            if (lex.src[lex.pos] == '\n') {
                lex.pos++;
                lex.line++;
                lex.column=0;
                break;
            }
            lex.pos++;
            lex.column++;
        }
        //if (lex.pos != lex.size) { throw std::runtime_error("comment not terminated by newline"); }
    } else {
        throw std::runtime_error("skipComment called on non comment char: " + std::string(1, lex.src[lex.pos]));
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

Token lexQuote(Lexer& lex) {
    int startColumn = lex.column;
    if (lex.pos < lex.size) {
        switch(lex.src[lex.pos]){
            case '\'':
                lex.pos++;
                lex.column++;
                return {TokenKind::QUOTE, lex.line, startColumn};
            case '`':
                lex.pos++;
                lex.column++;
                return {TokenKind::QQUOTE, lex.line, startColumn};
            case ',':
                if (lex.pos + 1 < lex.size && lex.src[lex.pos + 1] == '@') {
                    lex.pos += 2;
                    lex.column += 2;
                    return {TokenKind::UNQUOTESPLICE, lex.line, startColumn};
                }
                lex.pos++;
                lex.column++;
                return {TokenKind::UNQUOTE, lex.line, startColumn};
            default:
                throw std::runtime_error("lexQuote called on nonQuote char: " + std::string(1,lex.src[lex.pos]));
        }
    }
    throw std::runtime_error("fell off the end during lexing");
}

Token lexSymbol(Lexer& lex) {
    int startColumn = lex.column;
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
    std::string parse = "";
    if (isalpha(lex.src[lex.pos])) {
        while (lex.pos < lex.size &&
               ((isdigit(lex.src[lex.pos]) || isalpha(lex.src[lex.pos])) ||
                lex.src[lex.pos] == '-' || lex.src[lex.pos] == '?')) {
            if (lex.src[lex.pos] == '-' &&
                lex.pos + 1 < lex.size &&
                lex.src[lex.pos + 1] == '>') {
                break;
            }
            parse+=lex.src[lex.pos];
            lex.pos++;
            lex.column++;
        }
    } else if (opsyms.contains(lex.src[lex.pos])) {
         while(lex.pos < lex.size && opsyms.contains(lex.src[lex.pos])) {
            parse+=lex.src[lex.pos];
            lex.pos++;
            lex.column++;
         }
    } else {
        throw std::runtime_error("invalid symbol start char: " + std::string(1, lex.src[lex.pos]));
    }
    if (type_idents.contains(parse)) {
        return {TokenKind::TYPE_IDENT, Value(parse), lex.line, startColumn};
    } else if (parse == "quote") {
        return {TokenKind::QUOTE, lex.line, startColumn};
    } else if (parse == "qquote") {
        return {TokenKind::QQUOTE, lex.line, startColumn};
    } else if (parse == "unquote") {
        return {TokenKind::UNQUOTE, lex.line, startColumn};
    } else if (parse == "unquote-splice") {
        return {TokenKind::UNQUOTESPLICE, lex.line, startColumn};
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
    this->current = advance();
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

Token lexPlaceholder(Lexer& lex) {
    int startColumn = lex.column;
    if (lex.pos < lex.size) {
        lex.pos++;
        lex.column++;
        return{TokenKind::PLACEHOLDER, lex.line, startColumn};
    }
    throw std::runtime_error{"lexPlaceholder called after end of source"};
}

Token Lexer::advance(){
    char cur, nxt;
    if (pos >= size) { 
        return eof;
    } else if (pos < size){
        while(true) {
            if (pos >= size) {
                return eof;
            }
            nxt = '\0';                                  //reset nxt to end of string
            cur = this->src[this->pos];                  //set current 
            if (pos < size-1) { nxt = this->src[pos+1]; } //set nxt if there is a next char
            if (isspace(cur)){
                skipWhitespace(*this);
                continue;
            } else if (isdigit(cur) || (cur == 'i' && nxt != 'n') || (nxt != '\0' && cur == '-' && (isdigit(nxt) || nxt == 'i'))) {
                return lexNumber(*this); 
                break;
            } else if (cur == '(' || cur == ')') {
                return lexParen(*this);
                break; 
            } else if (cur == '-' && nxt == '>') {
                return lexArrow(*this);
                break;   
            } else if (isalpha(cur) || opsyms.contains(cur)) {
                return lexSymbol(*this); 
                break;  
            } else if (cur == '"') {
                return lexString(*this); 
                break;  
            }else if (cur == ':') {
                return lexColon(*this);
                break;   
            } else if (cur == '.') {
                return lexDot(*this);
                break;   
            } else if (cur == '#') {
                return lexBool(*this); 
                break;
            } else if (cur == '_') {
                return lexPlaceholder(*this);
                break;
            } else if (quoteStarts.contains(cur)){
                return lexQuote(*this);
                break;
            } else if (cur == ';') {
                skipComment(*this);
                continue;
            } else {
                //TODO
                throw std::runtime_error{"unable to lex character: " + std::string(1,cur)};
            }
        }
    } else {
        return eof;
    }
}

void Lexer::swapCurrent(Token t) {
    current = std::move(t);
    has_previous = false;
}

const Token& Lexer::peek(std::size_t lookahead) {
    if (lookahead == 0) {
        return current;
    }
    ensure(lookahead);
    return buffer[lookahead - 1];
}

Token Lexer::next() {
    Token old = current;
    if (!buffer.empty()) {
        current = buffer.front();
        buffer.pop_front();
    } else {
        current = advance();
    }
    previous = old;
    has_previous = true;
    return old;
}

void Lexer::backup() {
    if (!has_previous) {
        return;
    }
    buffer.push_front(current);
    current = previous;
    has_previous = false;
}

void Lexer::ensure(std::size_t i) {
    while (buffer.size() < i) {
        buffer.push_back(advance());
    }
}
