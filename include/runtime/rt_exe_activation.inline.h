#ifndef AFUN_RT_EXE_ACTIVATION_INLINE_H
#define AFUN_RT_EXE_ACTIVATION_INLINE_H
#include "rt_exe_activation.h"

namespace aFunrt {
    ExeActivation::ExeActivation(const aFuncode::Code &code, aFuncore::Inter &inter_) : NormalActivation(inter_), start{code.getByteCode()}, next{code.getByteCode()} {

    }

    ExeActivation::ExeActivation(const aFuncode::Code::ByteCode *code, aFuncore::Inter &inter_) : NormalActivation(inter_), start{code}, next{code} {

    }

    const aFuncode::Code::ByteCode *ExeActivation::getStart() const{
        return start;
    }
}

#endif //AFUN_RT_EXE_ACTIVATION_INLINE_H
