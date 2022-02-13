#ifndef AFUN_RT_FUNC_ACTIVATION_INLINE_H
#define AFUN_RT_FUNC_ACTIVATION_INLINE_H
#include "rt_func_activation.h"

namespace aFunrt {
    FuncActivation::FuncActivation(const aFuncode::Code::ByteCode *code, aFuncore::Inter &inter_) : NormalActivation(inter_), call{code} {

    }
}

#endif //AFUN_RT_FUNC_ACTIVATION_INLINE_H
