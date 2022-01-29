#ifndef AFUN_VALUE_INLINE_H
#define AFUN_VALUE_INLINE_H
#include "value.h"

namespace aFuncore {
    inline Function::Function(const std::string &type_, Inter &inter_) : Object(type_ + ":Function", inter_) {

    }

    inline bool Function::isInfix() {
        return false;
    }

    inline Literaler::Literaler(const std::string &type_, Inter &inter_) : Object(type_ + ":Literaler", inter_) {

    }

    inline CallBackVar::CallBackVar(const std::string &type_, Inter &inter_) : Object(type_ + ":CallBackVar", inter_) {

    }

    inline bool CallBackVar::isCallBack(Inter &inter, Activation &activation) {
        return true;
    }
};

#endif //AFUN_VALUE_INLINE_H
