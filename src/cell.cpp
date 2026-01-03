#include <memory>
#include <variant>
#include <string>
#include <iostream>
#include <type_traits>

struct Cell;

using List = std::shared_ptr<const Cell>;

struct Value {
      using V = std::variant<double, bool,const char*, std::string, List>;
      V v;

      Value() : v(List{}) {} //nil
      Value(double n) : v(n) {}
      explicit Value(bool b) : v(b) {}
      Value(const char* cs) : v(std::string(cs)) {}
      Value(std::string s) : v(std::move(s)) {}
      Value(List l) : v(std::move(l)) {}
};

struct Cell {
      Value car;
      Value cdr;

      Cell(Value a, Value d) : car(std::move(a)), cdr(std::move(d)) {}
};

inline List cons(Value a, Value d) {
      return std::make_shared<Cell>(std::move(a), std::move(d));
}

inline const Value& head(const List& lst) { return lst->car; }
inline const Value& tail(const List& lst) { return lst->cdr; }


bool isNil(const Value& val) { 
    if (auto p = std::get_if<List>(&val.v)){ return !*p;}
    else { return false; } 
}

bool isDouble(const Value& val) { 
    return std::holds_alternative<double>(val.v);
}

bool isBool(const Value& val) { 
    return std::holds_alternative<bool>(val.v);
}

bool isString(const Value& val) { 
    return std::holds_alternative<std::string>(val.v);
}

bool isList(const Value& val) { 
    return std::holds_alternative<List>(val.v);
}

bool isList(const List* lst) {
    return true;
}



Value nil = {};

std::ostream& operator<<(std::ostream& os, const Value& val) {
    if (isNil(val)) { os << "()"; }
    else if (isDouble(val)) { os << std::get<double>(val.v); }
    else if (isBool(val)) { os << std::get<bool>(val.v); }
    else if (isString(val)) { os << std::get<std::string>(val.v); }
    else if (isList(val)) { os << std::get<List>(val.v); }
    else { os << "invalid value" ;} //shouldnt be possible to reach
    return os;
}

std::ostream& operator<<(std::ostream& os, const List& lst) {
    List temp = lst; 
    os << '(';
    os << head(temp);
    if (!isList(tail(temp))) {
        os << " . " << tail(temp) << ')';
        return os;
    }
    else{
        temp = std::get<List>(tail(temp).v);
        while(!isNil(temp)){
            os << ", " << head(temp); 
            temp = std::get<List>(tail(temp).v);
        }
        os << ')';
        return os;
    }
}


int main() {
    List lst1 = cons(1.0, cons(2.0 ,nil));
    List lst2 = cons(1.0, 2.0);
    List lst3 = cons("boom", cons(false, cons(1.0, nil)));
    std::cout << std::boolalpha;
    std::cout << lst1 << std::endl << lst2 << std::endl << lst3 ;
    return 0;
}
