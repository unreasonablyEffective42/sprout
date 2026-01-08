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
//        "(lambda (x:(int->int) -> (vec int 4)) (+ x 1))",
//        "(lambda (x:(int->int) y:int ->(int->int->int)) body)",
        "(define x:int 3)",
        "(define x:(vec int 3) expr)",
        "(define foo (x:int y:int -> (int->int)) expr)",
        "(define foo (lambda (x:int y:int -> (int->int)) expr))"
    };
    for (std::string src : sources) {
        Lexer lex = Lexer(src); 
        std::cout << "String to parse: " << src << std::endl;
        std::cout << parse(lex) << std::endl;
    }
    return 0;
}
