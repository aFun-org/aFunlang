#ifndef AFUN_ENV_VAR_INLINE_H
#define AFUN_ENV_VAR_INLINE_H
#include "env-var.h"

namespace aFuncore {
    size_t EnvVarSpace::getCount() {
        std::unique_lock<std::mutex> mutex{lock};
        return var.size();
    }
}

#endif //AFUN_ENV_VAR_INLINE_H
