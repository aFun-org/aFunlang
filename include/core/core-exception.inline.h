#ifndef AFUN_CORE_EXCEPTION_INLINE_H
#define AFUN_CORE_EXCEPTION_INLINE_H
#include <utility>

#include "core-exception.h"

namespace aFuncore {
    inline aFuncoreException::aFuncoreException(const std::string &msg) : aFunException{msg} {

    }

    inline EnvironmentDestructException::EnvironmentDestructException() : aFuncoreException("Environment Destruct Error") {

    }

    inline RuntimeError::RuntimeError(const std::string &msg, std::string type_) : aFuncoreException(msg), type{std::move(type_)} {

    }

    inline const std::string &RuntimeError::getType() const {
        return type;
    }

    inline ArgumentError::ArgumentError() : RuntimeError("Argument mismatch", "ArgumentError") {

    }
}

#endif //AFUN_CORE_EXCEPTION_INLINE_H
