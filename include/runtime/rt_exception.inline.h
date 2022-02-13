#ifndef AFUN_RT_EXCEPTION_INLINE_H
#define AFUN_RT_EXCEPTION_INLINE_H
#include "rt_exception.h"

namespace aFunrt {
    aFunrtException::aFunrtException(const std::string &msg) : aFunException{msg} {

    }

    RuntimeError::RuntimeError(const std::string &msg, std::string type_) : aFunrtException(msg), type{std::move(type_)} {

    }

    const std::string &RuntimeError::getType() const {
        return type;
    }

    ArgumentError::ArgumentError() : RuntimeError("Argument mismatch", "ArgumentError") {

    }
}

#endif //AFUN_RT_EXCEPTION_INLINE_H
