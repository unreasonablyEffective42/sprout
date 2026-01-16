#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "ast_fwd.h"
#include "cell.h"
#include "token.h"
#include "value.h"
#include <iostream>
#include <unordered_map>
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
  // todo
};

/*
number: int, rational, float, complex
text: char, string
bool: bool
fun (t->t)
  */
struct TypeConstSig;
struct FunctSig;

struct Type {
  using T = std::variant<double, int, Rational, Complex, bool, char,
                         std::string, FunctSig, TypeConstSig>;
  T t;
};

struct FunctSig {
  std::vector<Cell<Value, Value>> params;
  std::vector<Type> signature;
};

struct TypeConstructSig {
  std::string name;
  Type datatype;
  Type size;
};
struct Tp {};

struct TypeFrame {
  std::unordered_map<std::string, Type> bindings;
};

struct TypeEnv {
  std::stack<TypeFrame> frames;
};

#endif // !TYPE_CHECKER_H
