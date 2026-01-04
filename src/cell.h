#ifndef SPROUT_LANG_CELL_H
#define SPROUT_LANG_CELL_H

#include "value.h"

struct Cell {
    Value car;
    Value cdr;

    Cell(Value a, Value d);
};

List cons(Value a, Value d);

const Value& head(const List& lst);
const Value& tail(const List& lst);

std::ostream& operator<<(std::ostream& os, const List& lst);

#endif
