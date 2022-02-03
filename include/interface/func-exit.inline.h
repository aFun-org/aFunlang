#ifndef AFUN_FUNC_EXIT_INLINE_H
#define AFUN_FUNC_EXIT_INLINE_H

#include "func-exit.h"

namespace aFunit {
    inline ExitFunction::ExitFunction(aFuncore::Inter &inter_) : Object("Function", inter_) {

    }

    inline ExitFunction::ExitFunction(aFuncore::Environment &env_) : Object("Function", env_) {

    }
}

#endif //AFUN_FUNC_EXIT_INLINE_H
