#include "cell.h"

#include <iostream>

std::ostream &operator<<(std::ostream &os, const List &lst) {
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
