#include "typechecker.h"
#include "ast.h"
#include "token.h"
#include "value.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <vector>

/* work in progress does not compile much TODO */
Type::Type(int i) { t = i; };
Type::Type(Rational r) { t = r; };
Type::Type(Complex c) { t = c; };
Type::Type(bool b) { t = b; };
Type::Type(char c) { t = c; };
Type::Type(std::string str) { t = str; };
Type::Type(std::shared_ptr<FunctSig> f) { t = f; };
Type::Type(std::shared_ptr<TypeConstSig> c) { t = c; };
Type::Type(std::shared_ptr<TypeVar> v) { t = v; };

TypeVar::TypeVar(std::string str) { name = str; };

FunctSig::FunctSig(std::vector<Cell<Value, Value>> params_,
                   std::vector<Type> signature_)
    : params(std::move(params_)), signature(std::move(signature_)) {}

TypeConstSig::TypeConstSig(std::string name_, std::vector<Type> typeArgs_,
                           std::vector<int> natArgs_)
    : name(std::move(name_)), typeArgs(std::move(typeArgs_)),
      natArgs(std::move(natArgs_)) {}

TypeFrame::TypeFrame(std::unordered_map<std::string, Type> bindings_)
    : bindings(std::move(bindings_)) {}

TypeEnv::TypeEnv(std::stack<TypeFrame> frames_) { frames = frames_; }

void insertBinding(std::string symbol, Type type, TypeFrame &frame) {
    frame.bindings.insert({symbol, type});
}

void insertFrame(TypeFrame &frame, TypeEnv &env) { env.frames.push(frame); }

Type makeFunctSig(const TokenNode &root) {}

Type makeTypeConstSig(const TokenNode &root) {}

void parseDefineV(const TokenNode &root) {
    std::size_t toks = size(root);
    if (toks != 4) {
        std::ostringstream oss;
        oss << root;
        throw std::runtime_error("expression define malformed, found:" +
                                 oss.str());
    }
    TokenList start = tail(asTokenList(root));
    std::string sym =
        std::get<std::string>((*(std::get<Token>(head(start))).value).v);
    start = tail(start);
    Type t;
    if (isTokenNodeToken(head(start))) {
        Token tok = std::get<Token>(head(start));
        if (isString(*tok.value)) {
            t = Type(std::get<std::string>((*tok.value).v));
        } else if (isAstPtr(*tok.value)) {
            TokenNode temp = (std::get<AstPtr>((*tok.value).v))->node;

        } else {
            throw std::runtime_error("invalid value in type ident:");
        }
    }
    Value v;
    while (start != TokenList{}) {
    }
}
