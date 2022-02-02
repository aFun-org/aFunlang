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

    class RuntimeError : public aFuncoreException {
        std::string type;
    public:
        inline RuntimeError(const std::string &msg, std::string type);
        inline const std::string &getType() const;
    };

    class ArgumentError : public RuntimeError {
        std::string type;
    public:
        inline ArgumentError();
    };
}

#include "core-exception.inline.h"

#endif //AFUN_CORE_EXCEPTION_H
