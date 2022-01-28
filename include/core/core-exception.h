#ifndef AFUN_CORE_EXCEPTION_H
#define AFUN_CORE_EXCEPTION_H
#include "aFuntool.h"

namespace aFuncore {
    class aFuncoreException : public aFuntool::aFunException {
    public:
        inline explicit aFuncoreException(const std::string &msg);
    };

    class EnvironmentDestructException : public aFuncoreException {
    public:
        inline EnvironmentDestructException();
    };

}

#include "core-exception.inline.h"

#endif //AFUN_CORE_EXCEPTION_H
