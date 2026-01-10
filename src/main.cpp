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
#include <utility>

struct ParseCase {
    std::string src;
    bool expect_error;
};

void printParseReference() {
    std::vector<std::string> samples = {
        "(lambda (x:int -> int) (+ x 1))",
        "(lambda (x:(int->int) -> (vec int 4)) (+ x 1))",
        "(cond (#t 3) (#f 4))",
        "(define x:int 3)",
        "(define x:(vec int 3) expr)",
        "(define foo (x:int y:int -> (int->int)) expr)",
        "(define foo (lambda (x:int y:int -> (int->int)) expr))",
        "(let ((x:int 0) (y:(int->int) (lambda (z:int -> int) expr))) body)",
        "(letr loop ((x:(int->int) fun1) (y:(int->int) fun2)) body)",
        "(quote (1 2 3))",
        "'(1 2 3)",
        "`(1 ,(+ 2 3) ,@(4 5 6))",
        "(define bar (x:float y:((int->bool)->(bool->float)->(float->bool->int)) -> (float->float)) body)"
    };

    std::cout << "Parse Reference" << std::endl;
    for (const auto& src : samples) {
        Lexer lex(src);
        TokenNode tree = parse(lex);
        if (!validateQuote(tree, 0)) {
            throw std::runtime_error("unquote outside quasiquote");
        }
        std::cout << "String to parse: " << src << std::endl;
        std::cout << tree << std::endl;
    }
}

void testParse() {
    std::vector<ParseCase> cases = {
        {"(lambda (x:int -> int) (+ x 1))", false},
        {"(lambda (x:(int->int) -> (vec int 4)) (+ x 1))", false},
        {"(cond (#t 3) (#f 4))", false},
        {"(define x:int 3)", false},
        {"(define x:(vec int 3) expr)", false},
        {"(define foo (x:int y:int -> (int->int)) expr)", false},
        {"(define foo (lambda (x:int y:int -> (int->int)) expr))", false},
        {"(let ((x:int 0) (y:(int->int) (lambda (z:int -> int) expr))) body)", false},
        {"(letr loop ((x:(int->int) fun1) (y:(int->int) fun2)) body)", false},
        {"(quote (1 2 3))", false},
        {"'(1 2 3)", false},
        {"`(1 ,(+ 2 3) ,@(4 5 6))", false},
        // Error cases.
        {"(lambda (x:int -> int) (+ x 1)", true},
        {"(cond (#t 1) (#f))", true},
        {"(let ((x:int 0) (y:(int->int))) body)", true},
        {"(define x: 3)", true},
        {"(define foo (x:int y:int -> (int->int))", true},
        {"(,x)", true}
    };

    for (const auto& tc : cases) {
        std::cout << "String to parse: " << tc.src << std::endl;
        try {
            Lexer lex(tc.src);
            TokenNode tree = parse(lex);
            if (!validateQuote(tree, 0)) {
                throw std::runtime_error("unquote outside quasiquote");
            }
            std::cout << tree << std::endl;
            if (tc.expect_error) {
                std::cout << "ERROR: expected failure but parsed successfully" << std::endl;
            }
        } catch (const std::runtime_error& e) {
            std::cout << "Parse error: " << e.what() << std::endl;
            if (!tc.expect_error) {
                std::cout << "ERROR: expected success but failed" << std::endl;
            }
        }
    }
}

void testLexArrow() {
    std::string sources[] {
        "int->int",
        "intensity"
    };
    for (std::string src : sources) {
        Lexer lex(src);
        std::cout << "Lexer test: " << src << std::endl;
        while (lex.peek(0).kind != TokenKind::END) {
            std::cout << lex.next() << std::endl;
        }
        std::cout << "-------------" << std::endl;
    }
}

int main() {
//    testLexArrow();
//    printParseReference();
//    testParse();
    return 0;
}
