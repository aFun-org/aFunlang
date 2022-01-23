#ifndef AFUN_CORE_EXCEPTION_INLINE_H
#define AFUN_CORE_EXCEPTION_INLINE_H
#include "core-exception.h"

namespace aFuncore {
    inline const char *EnvironmentDestructException::what() {
        return message;
    }
}

#endif //AFUN_CORE_EXCEPTION_INLINE_H
