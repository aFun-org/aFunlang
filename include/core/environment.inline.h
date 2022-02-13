#ifndef AFUN_ENVIRONMENT_INLINE_H
#define AFUN_ENVIRONMENT_INLINE_H
#include "environment.h"

namespace aFuncore {
    EnvVarSpace &Environment::getEnvVarSpace() {
        return env_var;
    }
}

#endif //AFUN_ENVIRONMENT_INLINE_H
