#ifndef AFUN_ENV_VAR_INLINE_H
#define AFUN_ENV_VAR_INLINE_H
#include "env-var.h"

namespace aFuncore {
    inline size_t EnvVarSpace::getCount() const{
        return var.size();
    }
}

#endif //AFUN_ENV_VAR_INLINE_H
