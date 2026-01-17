#include "cell.h"
#include "lexer.h"
#include "parser.h"
#include "rational.h"
#include "token.h"
#include "value.h"

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct ParseCase {
    std::string src;
    bool expect_error;
};

void printParseReference() {
    std::vector<std::string> samples = {
        "(tlambda (A) body)", "(tlambda (A B) (lambda (x:A y:B -> B) body))",
        "(lambda (x:A y:B ->a) body)",
        "(lambda (x:(A->B->A) y:int -> (A->B)) body)",
        "(lambda (x:int -> int) (+ x 1))",
        "(lambda (x:(int->int) -> (vec int 4)) (+ x 1))",
        "(tapply expr int A (A->B->C))",
        /*
        "(cond (#t 3) (#f 4))",
        "(define x:int 3)",
        "(define x:(vec int 3) expr)",
        "(define foo (x:int y:int -> (int->int)) expr)",
        "(define foo (lambda (x:int y:int -> (int->int)) expr))",
        */
        "(let ((x:int 0) (y:(A->int->A) (lambda (z:int -> int) expr))) body)",
        "(letr loop ((x:(int->int) fun1) (y:(int->int) fun2)) body)",
        "(quote (1 2 3))", "'(1 2 3)",
        "(define id:(forall (A) (A -> A)) (tlambda (A) (lambda (x:A -> A) x)))",
        "(let ((f:(int->int->int) (foo _ 2 _ 3))) (f 3 5))", "(1 . 3)",
        "(1 2 3 . 4)", "(match m ((Just x) x) ((Nothing) 0) (else -1))",
        "(match v ((Just (Just x)) x) ((Just (Nothing)) 1) ((Nothing) 0) (else "
        "-1))",
        "(match xs ((a b c) (+ a (+ b c))) (else 0))",
        "(match xs ((x y . rest) (cons x rest)) (else xs))",
        "(match p ((x . y) x) (else 0))",
        "(match xs (((a b) c . rest) c) (else -1))",
        "(match xs ((x . _) x) (else 0))",
        "(match xs (42 x) (_ doThing) (else 0))",
        "(data Maybe (A) (Nothing) (Just (A)))",
        "(data List (A) (Nil) (Cons (A (List A))))"
        //"`(1 ,(+ 2 3) ,@(4 5 6))",
        //"(define bar (x:float
        // y:((int->bool)->(bool->float)->(float->bool->int))
        //-> (float->float)) body)"
    };

    std::cout << "Parse Reference" << std::endl;
    for (const auto &src : samples) {
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
        {"(let ((x:int 0) (y:(int->int) (lambda (z:int -> int) expr))) body)",
         false},
        {"(letr loop ((x:(int->int) fun1) (y:(int->int) fun2)) body)", false},
        {"(quote (1 2 3))", false},
        {"'(1 2 3)", false},
        {"`(1 ,(+ 2 3) ,@(4 5 6))", false},
        {"(tlambda (A) body)", false},
        {"(tlambda (A B) (lambda (x:A y:B -> B) body))", false},
        {"(tapply expr int A (A->B->C))", false},
        {"(define id:(forall (A) (A -> A)) (tlambda (A) (lambda (x:A -> A) "
         "x)))",
         false},
        {"(match m ((Just x) x) ((Nothing) 0) (else -1))", false},
        {"(match xs ((a b c) (+ a (+ b c))) (else 0))", false},
        {"(match xs ((x y . rest) (cons x rest)) (else xs))", false},
        {"(match xs (((a b) c . rest) c) (else -1))", false},
        {"(data Maybe (A) (Nothing) (Just (A)))", false},
        {"(data List (A) (Nil) (Cons (A (List A))))", false},
        // Error cases.
        {"(lambda (x:int -> int) (+ x 1)", true},
        {"(cond (#t 1) (#f))", true},
        {"(let ((x:int 0) (y:(int->int))) body)", true},
        {"(define x: 3)", true},
        {"(define foo (x:int y:int -> (int->int))", true},
        {"(,x)", true},
        {"(tlambda A body)", true},
        {"(tapply expr)", true},
        {"(match xs ((a b c) 1) (else))", true},
        {"(data (A) (Nothing))", true},
        {"(data Maybe (A) (Just A))", true},
        {"(data Maybe A (Just (A)))", true}};

    for (const auto &tc : cases) {
        std::cout << "String to parse: " << tc.src << std::endl;
        try {
            Lexer lex(tc.src);
            TokenNode tree = parse(lex);
            if (!validateQuote(tree, 0)) {
                throw std::runtime_error("unquote outside quasiquote");
            }
            std::cout << tree << std::endl;
            if (tc.expect_error) {
                std::cout << "ERROR: expected failure but parsed successfully"
                          << std::endl;
            }
        } catch (const std::runtime_error &e) {
            std::cout << "Parse error: " << e.what() << std::endl;
            if (!tc.expect_error) {
                std::cout << "ERROR: expected success but failed" << std::endl;
            }
        }
    }
}

void testLexArrow() {
    std::string sources[]{"int->int", "intensity"};
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
    //  testParse();
    Lexer lex("(define f:(int->int) expr");
    std::cout << parse(lex);
    return 0;
}
