#ifndef SPROUT_LANG_VALUE_H
#define SPROUT_LANG_VALUE_H
// ast_fwd is a forward declaration of the AstPtr which wraps ast nodes to avoid
// circular dependencey issues
#include "ast_fwd.h"
#include "complex.h"
#include "rational.h"

#include <memory>
#include <ostream>
#include <string>
#include <variant>
/*
 * Value is a wrapper for a Variant over the core language types, the
 * monomorphic ones in addition to constructed types and Algebrati Data Types
 * using the astptr wrapper over astnodes
 */
struct Value;
template <typename CarT, typename CdrT = CarT> struct Cell;

/*
 * linked list/tree structure over Values, will be emitted as the final
 * intermediate representation after lowering before bytecode generation
 */
using List = std::shared_ptr<const Cell<Value, Value>>;

/*
 * wrapper struct over Symbols, ie labels in (define foo:type expr)  (let ((foo
 * expr) ... additional bindings) expr) foo is a Symbol
 */
struct Symbol {
    std::string name;
    Symbol(std::string name_);
};

// Function wrapper over the parameter list and the body expression
struct Function {
    AstPtr params;
    AstPtr body;
    //    EnvPtr env;
};

// Cond wrapper, list of clauses of form ((pred1 expr1) (pred2 expr2) ... (predn
// exprn))
struct Conditional {
    AstPtr clauses;
    //   EnvPtr env
};

struct Value {
    using V =
        std::variant<double, int, Rational, Complex, bool, char, std::string,
                     Symbol, Function, AstPtr, Conditional, List>;
    V v;

    Value();
    Value(int i);
    Value(double n);
    Value(Rational r);
    Value(Complex cx);
    Value(bool b);
    Value(char c);
    Value(std::string s);
    Value(Symbol sym);
    Value(Function fun);
    Value(Conditional cont);
    Value(AstPtr ptr);
    Value(List l);
};

bool isNil(const Value &val);
bool isDouble(const Value &val);
bool isBool(const Value &val);
bool isString(const Value &val);
bool isChar(const Value &val);
bool isList(const Value &val);
bool isInt(const Value &val);
bool isRational(const Value &val);
bool isSymbol(const Value &val);
bool isFunction(const Value &val);
bool isConditional(const Value &val);
bool isAstPtr(const Value &val);
bool isComplex(const Value &val);

extern Value nil;

// operator overloads
std::ostream &operator<<(std::ostream &os, const Value &val);
std::ostream &operator<<(std::ostream &os, const Symbol &sym);
std::ostream &operator<<(std::ostream &os, const Function &fun);
std::ostream &operator<<(std::ostream &os, const Conditional &cond);
bool operator==(const Symbol &a, const Symbol &b);
bool operator!=(const Symbol &a, const Symbol &b);
bool operator==(const Value &a, const Value &b);
bool operator==(const Value &a, const Value &b);
#endif
