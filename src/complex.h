#ifndef SPROUTLANG_COMPLEX_H
#define SPROUTLANG_COMPLEX_H

#include <ostream>
#include <cmath>
#include <stdexcept>
#include <string>

struct Complex {
      double re;
      double im;

      Complex(double r = 0, double i = 0) : re(r), im(i) {}
};

inline Complex conjugate(const Complex& a){
    return {a.re, -1*a.im};
}

inline double cabs(const Complex& a) {
    return std::sqrt(a.re*a.re + a.im*a.im);
}

inline double carg(const Complex& a) {
    return std::atan2(a.im, a.re);
}

inline Complex operator+(const Complex& a, const Complex& b) {
    return {a.re + b.re, a.im + b.im};
}

inline Complex operator-(const Complex& a, const Complex& b) {
    return {a.re - b.re, a.im - b.im};
}

inline Complex operator*(const Complex& a, const Complex& b) {
    return {a.re*b.re - a.im*b.im, a.re*b.im + a.im*b.re};
}

inline Complex operator/(const Complex& a, const Complex& b) {
    if (b.re == 0 && b.im == 0) { throw std::runtime_error("Error: cannot divide by zero complex number"); }
    double denom = b.re*b.re + b.im*b.im;
    return {(a.re*b.re + a.im*b.im)/denom, (a.im*b.re - a.re*b.im)/denom};
   
}

inline bool operator==(const Complex& a, const Complex& b){
    return a.re == b.re && a.im == b.im;
}

inline bool operator!=(const Complex& a, const Complex& b) {
     return a.re != b.re || a.im != b.im;
}


std::ostream& operator<<(std::ostream& os, const Complex& a);
Complex cFromString(const std::string& str);

#endif
