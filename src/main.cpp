#include "value.h"
#include "token.h"
#include "cell.h"
#include "rational.h"
#include "lexer.h"

#include <iostream>
#include <string>
#include <vector>


int main() {
    std::string src = "(23 hello 3.4 8/3 2+3i\n 8.34 -2-4i) ()\n \"this is a string\" -> : . #t #f \"s\" i int bool rational float complex char string symbol list vec ";
    Lexer lex = Lexer(src);
    Token current = lex.peek();
    while (current != Lexer::eof) {
        std::cout << current << std::endl;
        current = lex.next();
    }
    return 0;
}
