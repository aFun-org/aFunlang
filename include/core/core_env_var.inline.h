#ifndef AFUN_CORE_ENV_VAR_INLINE_H
#define AFUN_CORE_ENV_VAR_INLINE_H
#include "core_env_var.h"

namespace aFuncore {
    size_t EnvVarSpace::getCount() {
        std::unique_lock<std::mutex> mutex{lock};
        return var.size();
    }
}

#endif //AFUN_CORE_ENV_VAR_INLINE_H
