#ifndef RATIONAL_H
#define RATIONAL_H

#include <iosfwd>
#include <string>

struct Rational {
    int numerator;
    int denominator;
    Rational(int n, int d);
    explicit operator double() const {
        return static_cast<double>(numerator) / denominator;
    }
};

Rational rFromString(std::string str);
std::ostream& operator<<(std::ostream& os, const Rational& r);


inline bool operator==(const Rational& a, const Rational& b){
    return a.numerator == b.numerator && a.denominator == b.denominator;
}

inline bool operator!=(const Rational& a, const Rational& b){
    return a.numerator != b.numerator || a.denominator != b.denominator;
}

inline bool operator>(const Rational& a, const Rational& b){
    return static_cast<double>(a) > static_cast<double>(b);
}

inline bool operator<(const Rational& a, const Rational& b){
    return static_cast<double>(a) < static_cast<double>(b);
}

inline bool operator<=(const Rational& a, const Rational& b){
    return static_cast<double>(a) <=static_cast<double>(b);
}

inline bool operator>=(const Rational& a, const Rational& b){
    return static_cast<double>(a) >= static_cast<double>(b);
}
#endif
