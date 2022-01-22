#ifndef AFUN_INTER_INLINE_H
#define AFUN_INTER_INLINE_H
#include "inter.h"

namespace aFuncore {
    inline Inter::GcRecord *Inter::getGcRecord() const {
        return gc;
    }

    inline void Inter::pushActivation(Activation *new_activation) {
        activation = new_activation;
    }

    inline InterStatus Inter::getStatus() const {
        return status;
    }

    inline bool Inter::isExit() const {
        return (status == inter_exit || status == inter_stop);
    }

    inline ProtectVarSpace *Inter::getProtectVarSpace() const {
        return protect;
    }

    inline VarSpace *Inter::getGlobalVarSpace() const {
        return global;
    }

    inline VarList *Inter::getGlobalVarlist() const {
        return global_varlist;
    }

    inline Activation *Inter::getActivation() const {
        return activation;
    }

    inline EnvVarSpace &Inter::getEnvVarSpace() {
        return envvar;
    }
}

#endif //AFUN_INTER_INLINE_H
