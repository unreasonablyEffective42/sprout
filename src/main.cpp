#include "value.h"
#include "token.h"
#include "cell.h"
#include "rational.h"
#include "lexer.h"

#include <iostream>
#include <string>
#include <vector>


int main() {
    const std::vector<std::string> sources = {
        "(23 hello 3.4 8/3 2+3i)",
        "(define (x : int) (cons 1 2))",
        "\"this is a string\" 'x #t #f -> : .",
        "(list vec rational float complex char string symbol list)",
        "`(,thing1 ,thing2 ,@(some stuff)",
        "+ - / <+ ==> && *-*",
        "(some stuff) ;a comment"
    };

    for (const auto& src : sources) {
        std::cout << "SOURCE: " << src << std::endl;
        Lexer lex(src);
        Token current = lex.next();
        while (current.kind != TokenKind::END) {
            std::cout << current << std::endl;
            current = lex.next();
        }
        std::cout << "----" << std::endl;
    }
    return 0;
}
