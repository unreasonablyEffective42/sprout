#include"fraction.h"

#include<iostream>
#include<numeric>
#include<stdexcept>

Fraction::Fraction(int n, int d) {
    if (d == 0) { throw std::runtime_error("faction with zero denominator"); }
    int g = std::gcd(n,d);
    numerator = n/g;
    denominator = d/g;
}

std::ostream& operator<<(std::ostream& os, const Fraction& f) {
    os << f.numerator << '/' << f.denominator ;
    return os;
}
