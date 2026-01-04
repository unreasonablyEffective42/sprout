#include "cell.h"

#include <iostream>

Cell::Cell(Value a, Value d) : car(std::move(a)), cdr(std::move(d)) {}

List cons(Value a, Value d) {
    return std::make_shared<Cell>(std::move(a), std::move(d));
}

const Value& head(const List& lst) { return lst->car; }
const Value& tail(const List& lst) { return lst->cdr; }

std::ostream& operator<<(std::ostream& os, const List& lst) {
    List temp = lst;
    os << '(';
    while (temp) {
        if (!isList(tail(temp))) {
            os << head(temp) << " . " << tail(temp) << ')';
            return os;
        }
        os << head(temp);
        temp = std::get<List>(tail(temp).v);
        if (temp) {
            os << ", ";
        }
    }
    os << ')';
    return os;
}


