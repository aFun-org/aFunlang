#ifndef AFUN_CORE_EXCEPTION_H
#define AFUN_CORE_EXCEPTION_H
#include "aFuntool.h"

namespace aFuncore {
    class aFuncoreException : public aFuntool::aFunException {
    public:
        AFUN_INLINE explicit aFuncoreException(const std::string &msg);
    };

    class EnvironmentDestructException : public aFuncoreException {
    public:
        AFUN_INLINE EnvironmentDestructException();
    };
}

#include "core_exception.inline.h"

#endif //AFUN_CORE_EXCEPTION_H
