#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "ast_fwd.h"
#include "cell.h"
#include "rational.h"
#include "token.h"
#include "value.h"
#include <complex.h>
#include <iostream>
#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>
/*
  * String to parse: (define x:int 3)
(TOKEN[ kind=DEFINE ], TOKEN[ kind=SYMBOL value=x ], TOKEN[ kind=TYPE_IDENT
value=int ], TOKEN[ kind=NUMBER value=3 ])

String to parse: (define x:(vec int 3) expr)
(TOKEN[ kind=DEFINE ], TOKEN[ kind=SYMBOL value=x ], TOKEN[ kind=TYPE_IDENT
value=(TOKEN[ kind=TYPE_IDENT value=vec ], TOKEN[ kind=TYPE_IDENT value=int ],
TOKEN[ kind=NUMBER value=3 ]) ], TOKEN[ kind=SYMBOL value=expr ])

String to parse: (define foo (x:int y:int -> (int->int)) expr)
(TOKEN[ kind=DEFINE ], TOKEN[ kind=SYMBOL value=foo ], TOKEN[ kind=TYPE_IDENT
value=(TOKEN[ kind=PARAM_LIST ], (TOKEN[ kind=SYMBOL value=x ], TOKEN[
kind=TYPE_IDENT value=int ]), (TOKEN[ kind=SYMBOL value=y ], TOKEN[
kind=TYPE_IDENT value=int ]), TOKEN[ kind=RETURN_TYPE value=TOKEN[
kind=TYPE_IDENT value=(TOKEN[ kind=TYPE_IDENT value=int ], TOKEN[
kind=TYPE_IDENT value=int ]) ] ]) ], TOKEN[ kind=SYMBOL value=expr ])

String to parse: (define foo (lambda (x:int y:int -> (int->int)) expr))
(TOKEN[ kind=DEFINE ], TOKEN[ kind=SYMBOL value=foo ], (TOKEN[ kind=LAMBDA ],
(TOKEN[ kind=PARAM_LIST ], (TOKEN[ kind=SYMBOL value=x ], TOKEN[ kind=TYPE_IDENT
value=int ]), (TOKEN[ kind=SYMBOL value=y ], TOKEN[ kind=TYPE_IDENT value=int
]), TOKEN[ kind=RETURN_TYPE value=TOKEN[ kind=TYPE_IDENT value=(TOKEN[
kind=TYPE_IDENT value=int ], TOKEN[ kind=TYPE_IDENT value=int ]) ] ]), TOKEN[
kind=SYMBOL value=expr ]))

String to parse: (let ((x:int 0) (y:(int->int) (lambda (z:int -> int) expr)))
body) (TOKEN[ kind=LET ], TOKEN[ kind=SYMBOL ], (TOKEN[ kind=LET_BINDING
value=(TOKEN[ kind=SYMBOL value=x ], TOKEN[ kind=TYPE_IDENT value=int ], TOKEN[
kind=NUMBER value=0 ]) ], TOKEN[ kind=LET_BINDING value=(TOKEN[ kind=SYMBOL
value=y ], TOKEN[ kind=TYPE_IDENT value=(TOKEN[ kind=TYPE_IDENT value=int ],
TOKEN[ kind=TYPE_IDENT value=int ]) ], (TOKEN[ kind=LAMBDA ], (TOKEN[
kind=PARAM_LIST ], (TOKEN[ kind=SYMBOL value=z ], TOKEN[ kind=TYPE_IDENT
value=int ]), TOKEN[ kind=RETURN_TYPE value=TOKEN[ kind=TYPE_IDENT value=int ]
]), TOKEN[ kind=SYMBOL value=expr ])) ]), TOKEN[ kind=SYMBOL value=body ])

  */
struct TypeErr {
    // TODO
};

/*
number: int, rational, float, complex
text: char, string
bool: bool
fun (t->t)
  */
struct TypeConstSig;
struct FunctSig;
struct TypeVar;

struct Type {
    using T =
        std::variant<double, int, Rational, Complex, bool, char, std::string,
                     std::shared_ptr<FunctSig>, std::shared_ptr<TypeConstSig>,
                     std::shared_ptr<TypeVar>>;
    T t;
    Type();
    Type(int i);
    Type(Rational r);
    Type(Complex c);
    Type(bool b);
    Type(char c);
    Type(std::string str);
    Type(std::shared_ptr<FunctSig> f);
    Type(std::shared_ptr<TypeConstSig> c);
    Type(std::shared_ptr<TypeVar> v);
};

Type makeFunctSig(const TokenNode &root);

Type makeTypeConstSig(const TokenNode &root);

struct TypeVar {
    std::string name;
    TypeVar();
    TypeVar(std::string str);
};

struct FunctSig {
    std::vector<Cell<Value, Value>> params;
    std::vector<Type> signature;
    FunctSig();
    FunctSig(std::vector<Cell<Value, Value>> params_,
             std::vector<Type> signature_);
};

struct TypeConstSig {
    std::string name;
    std::vector<Type> typeArgs;
    std::vector<int> natArgs;

    TypeConstSig();
    TypeConstSig(std::string name_, std::vector<Type> typeArgs_,
                 std::vector<int> natArgs_);
};

struct TypeFrame {
    std::unordered_map<std::string, Type> bindings;
    TypeFrame();
    TypeFrame(std::unordered_map<std::string, Type> bindings_);
};

void insertBinding(std::string symbol, Type type, TypeFrame frame);

struct TypeEnv {
    std::stack<TypeFrame> frames;
    TypeEnv();
    TypeEnv(std::stack<TypeFrame> frames_);
};

void insertFrame(TypeFrame &frame, TypeEnv &env);

#endif // !TYPE_CHECKER_H
