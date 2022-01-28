#ifndef AFUN_CORE_EXCEPTION_INLINE_H
#define AFUN_CORE_EXCEPTION_INLINE_H
#include "core-exception.h"

namespace aFuncore {
    inline aFuncoreException::aFuncoreException(const std::string &msg) : aFunException{msg} {

    }

    inline EnvironmentDestructException::EnvironmentDestructException() : aFuncoreException("Environment Destruct Error") {

    }
}

#endif //AFUN_CORE_EXCEPTION_INLINE_H
