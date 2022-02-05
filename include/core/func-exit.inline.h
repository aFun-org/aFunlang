#ifndef AFUN_FUNC_EXIT_INLINE_H
#define AFUN_FUNC_EXIT_INLINE_H

#include "func-exit.h"

namespace aFuncore {
    inline ExitFunction::ExitFunction(Inter &inter_) : Object("Function", inter_) {

    }

    inline ExitFunction::ExitFunction(Environment &env_) : Object("Function", env_) {

    }
}

#endif //AFUN_FUNC_EXIT_INLINE_H
