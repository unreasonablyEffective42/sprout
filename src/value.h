#ifndef SPROUT_LANG_VALUE_H
#define SPROUT_LANG_VALUE_H

#include "fraction.h"

#include <memory>
#include <ostream>
#include <string>
#include <variant>

struct Cell;

using List = std::shared_ptr<const Cell>;

struct Symbol {
    std::string name;
    Symbol(std::string label){
        this->name = label;
    }
};

struct Value {
    using V = std::variant<double, int, Fraction, bool, char, const char*, std::string, Symbol, List>;
    V v;

    Value();
    Value(int i);
    Value(double n);
    Value(Fraction f);
    Value(bool b);
    Value(char c);
    Value(const char* cs);
    Value(std::string s);
    Value(Symbol sym);
    Value(List l);
};

bool isNil(const Value& val);
bool isDouble(const Value& val);
bool isBool(const Value& val);
bool isString(const Value& val);
bool isChar(const Value& val);
bool isList(const Value& val);
bool isInt(const Value& val);
bool isFraction(const Value& val);
bool isSymbol(const Value& val);

extern Value nil;

std::ostream& operator<<(std::ostream& os, const Value& val);
std::ostream& operator<<(std::ostream& os, const Symbol& sym);

#endif
