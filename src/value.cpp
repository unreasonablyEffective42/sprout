#include "value.h"

#include <iostream>
#include <complex>


Value::Value() : v(List{}) {}
Value::Value(int i) : v(i) {}
Value::Value(double n) : v(n) {}
Value::Value(Rational r) : v(std::move(r)) {}
Value::Value(Complex cx) : v(std::move(cx)) {}
Value::Value(bool b) : v(b) {}
Value::Value(char c) : v(c) {}
Value::Value(std::string s) : v(std::move(s)) {}
Value::Value(Symbol sym) : v(std::move(sym)) {}
Value::Value(List l) : v(std::move(l)) {}

Symbol::Symbol(std::string name_){
    this->name = name_;
}
bool isNil(const Value& val) {
    if (auto p = std::get_if<List>(&val.v)) {
        return !*p;
    }
    return false;
}

bool isDouble(const Value& val) { return std::holds_alternative<double>(val.v); }
bool isBool(const Value& val) { return std::holds_alternative<bool>(val.v); }
bool isString(const Value& val) { return std::holds_alternative<std::string>(val.v); }
bool isChar(const Value& val) { return std::holds_alternative<char>(val.v); }
bool isList(const Value& val) { return std::holds_alternative<List>(val.v); }
bool isInt(const Value& val) { return std::holds_alternative<int>(val.v); }
bool isRational(const Value& val) { return std::holds_alternative<Rational>(val.v); }
bool isSymbol(const Value& val) { return std::holds_alternative<Symbol>(val.v); }
bool isComplex(const Value& val) { return std::holds_alternative<Complex>(val.v); }

Value nil = {};

std::ostream& operator<<(std::ostream& os, const Symbol& sym) {
    os << "Symbol: " << sym.name;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Value& val) {
    if (isNil(val)) {
        os << "()";
    } else if (isDouble(val)) {
        os << std::get<double>(val.v);
    } else if (isInt(val)) {
        os << std::get<int>(val.v);
    } else if (isBool(val)) {
        os << std::get<bool>(val.v);
    } else if (isString(val)) {
        os << std::get<std::string>(val.v);
    } else if (isChar(val)) {
        os << std::get<char>(val.v);
    } else if (isSymbol(val)) {
        os << std::get<Symbol>(val.v);
    } else if (isList(val)) {
        os << std::get<List>(val.v);
    } else if (isRational(val)) {
        os << std::get<Rational>(val.v);
    } else if (isComplex(val)) {
        os << std::get<Complex>(val.v);
    } else {
        os << "invalid value";
    }
    return os;
}

bool operator==(const Symbol& a, const Symbol& b) {
    return a.name == b.name;
}

bool operator!=(const Symbol& a, const Symbol& b) {
    return a.name != b.name;
}

bool operator==(const Value& a, const Value& b) {
    return a.v == b.v;
}
bool operator!=(const Value& a, const Value& b) {
    return a.v != b.v;
}
