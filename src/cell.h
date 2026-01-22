#ifndef SPROUT_LANG_CELL_H
#define SPROUT_LANG_CELL_H

#include "value.h"
#include <utility>

// lisp style cons cell for building list and tree structures, either slot can
// be an element or a cell itself

template <typename CarT, typename CdrT> struct Cell {
    CarT car;
    CdrT cdr;

    Cell(CarT a, CdrT d) : car(std::move(a)), cdr(std::move(d)) {}
};

template <typename CarT, typename CdrT>
inline std::shared_ptr<const Cell<CarT, CdrT>> cons(CarT a, CdrT d) {
    return std::make_shared<Cell<CarT, CdrT>>(std::move(a), std::move(d));
}

template <typename CarT, typename CdrT>
inline const CarT &head(const std::shared_ptr<const Cell<CarT, CdrT>> &lst) {
    return lst->car;
}

template <typename CarT, typename CdrT>
inline const CdrT &tail(const std::shared_ptr<const Cell<CarT, CdrT>> &lst) {
    return lst->cdr;
}

std::ostream &operator<<(std::ostream &os, const List &lst);

#endif
