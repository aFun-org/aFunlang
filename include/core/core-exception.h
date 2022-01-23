#ifndef AFUN_CORE_EXCEPTION_H
#define AFUN_CORE_EXCEPTION_H
#include "iostream"

namespace aFuncore {
    class EnvironmentDestructException : public std::exception {
        constexpr static const char *message = "Environment Destruct Error";
    public:
        inline virtual const char *what();
    };

}

#include "core-exception.inline.h"

#endif //AFUN_CORE_EXCEPTION_H
