#ifndef AFUN_VALUE_INLINE_H
#define AFUN_VALUE_INLINE_H
#include "value.h"

namespace aFuncore {
    inline bool Function::isInfix() {
        return false;
    }

    inline bool CallBackVar::isCallBack(Inter &inter, Activation &activation) {
        return true;
    }
};

#endif //AFUN_VALUE_INLINE_H
