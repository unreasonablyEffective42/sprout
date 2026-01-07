#include "value.h"
#include "token.h"
#include "cell.h"
#include "rational.h"
#include "lexer.h"
#include "parser.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

int main() { 
    std::string sources[] = {
        "(lambda (x:int -> int) (+ x 1))",
        "(lambda (x:(int->int) y:int ->(int->int->int)) body)",
    };
    for (std::string src : sources) {
        Lexer lex = Lexer(src); 
        std::cout << "String to parse: " << src << std::endl;
        std::cout << parse(lex) << std::endl;
    }
    return 0;
}
