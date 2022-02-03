#ifndef AFUN_FUNC_IMPORT_INLINE_H
#define AFUN_FUNC_IMPORT_INLINE_H
#include "func-import.h"

namespace aFunit {
    inline ImportFunction::ImportFunction(aFuncore::Inter &inter_) : Object("Function", inter_) {

    }

    inline ImportFunction::ImportFunction(aFuncore::Environment &env_) : Object("Function", env_) {

    }
}

#endif //AFUN_FUNC_IMPORT_INLINE_H
