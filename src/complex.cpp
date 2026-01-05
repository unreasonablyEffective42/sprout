#include "complex.h"

#include <iostream>
#include <cmath>
#include <stdexcept>

Complex cFromString(const std::string& str) {
    std::string s = str;
    if (s.empty() || s.back() != 'i') {
        throw std::invalid_argument("complex literal must end with 'i'");
    }
    s.pop_back(); // remove trailing 'i'

    std::size_t split = std::string::npos;
    for (std::size_t i = 1; i < s.size(); ++i) {
        if (s[i] == '+' || s[i] == '-') {
            split = i;
            break;
        }
    }

    std::string real;
    std::string imag;
    if (split == std::string::npos) {
        real = "0";
        imag = s;
    } else {
        real = s.substr(0, split);
        imag = s.substr(split);
    }

    if (imag == "+" || imag == "-" || imag.empty()) {
        imag += "1";
    }
    if (real.empty()) {
        real = "0";
    }

    return {std::stod(real), std::stod(imag)};
}

std::ostream& operator<<(std::ostream& os, const Complex& a){
    if (a.re == 0 && a.im == 0) {
        os << "0";
        return os;
    }
    if (a.re != 0) {
        os << a.re;
    }
    if (a.im != 0) {
        if (a.im > 0 && a.re != 0) {
            os << '+';
        }
        if (a.im == -1) {
            os << "-i";
        } else if (a.im == 1) {
            os << "i";
        } else {
            os << a.im << 'i';
        }
    }
    return os;
}

