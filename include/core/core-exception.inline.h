#ifndef AFUN_CORE_EXCEPTION_INLINE_H
#define AFUN_CORE_EXCEPTION_INLINE_H
#include <utility>

#include "core-exception.h"

namespace aFuncore {
    aFuncoreException::aFuncoreException(const std::string &msg) : aFunException{msg} {

    }

    EnvironmentDestructException::EnvironmentDestructException() : aFuncoreException("Environment Destruct Error") {

    }
}

#endif //AFUN_CORE_EXCEPTION_INLINE_H
