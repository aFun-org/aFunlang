#ifndef AFUN_CORE_ENVIRONMENT_INLINE_H
#define AFUN_CORE_ENVIRONMENT_INLINE_H
#include "core_environment.h"

namespace aFuncore {
    EnvVarSpace &Environment::getEnvVarSpace() {
        return env_var;
    }
}

#endif //AFUN_CORE_ENVIRONMENT_INLINE_H
