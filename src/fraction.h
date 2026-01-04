#ifndef FRACTION_H
#define FRACTION_H

#include <iosfwd>

struct Fraction {
    int numerator;
    int denominator;
    Fraction(int n, int d);
};

std::ostream& operator<<(std::ostream& os, const Fraction& f);

#endif
