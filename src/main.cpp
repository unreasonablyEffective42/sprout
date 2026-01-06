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
        "(cond (#t 3) (#f 4))",
        "(1 2 (3 4 5))" 
    };
    for (std::string src : sources) {
        Lexer lex = Lexer(src); 
        std::cout << "String to parse: " << src << std::endl;
        std::cout << parse(lex) << std::endl;
    }
    return 0;
}
