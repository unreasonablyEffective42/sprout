#include"rational.h"

#include<iostream>
#include<numeric>
#include<stdexcept>
#include<string>

Rational::Rational(int n, int d) {
    if (d == 0) { throw std::runtime_error("faction with zero denominator"); }
    int g = std::gcd(n,d);
    numerator = n/g;
    denominator = d/g;
    if (denominator < 0) {
        numerator *= -1;
        denominator *= -1;
    }
}

Rational rFromString(std::string str) {
    int split = str.find('/');
    std::string numerator = str.substr(0,split);
    std::string denominator = str.substr(split+1);
    return {std::stoi(numerator),std::stoi(denominator)};
}

std::ostream& operator<<(std::ostream& os, const Rational& f) {
    os << f.numerator << '/' << f.denominator ;
    return os;
}

