#ifndef AFUN_RT_EXCEPTION_H
#define AFUN_RT_EXCEPTION_H
#include "aFuntool.h"

namespace aFunrt {
    class aFunrtException : public aFuntool::aFunException {
    public:
        AFUN_INLINE explicit aFunrtException(const std::string &msg);
    };

    class RuntimeError : public aFunrtException {
        std::string type;
    public:
        AFUN_INLINE RuntimeError(const std::string &msg, std::string type);
        AFUN_INLINE const std::string &getType() const;
    };

    class ArgumentError : public RuntimeError {
        std::string type;
    public:
        AFUN_INLINE ArgumentError();
    };
}

#include "rt-exception.inline.h"

#endif //AFUN_RT_EXCEPTION_H
