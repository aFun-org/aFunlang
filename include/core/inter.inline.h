#ifndef AFUN_INTER_INLINE_H
#define AFUN_INTER_INLINE_H
#include "inter.h"

namespace aFuncore {
    inline Environment &Inter::getEnvironment() {
        return env;
    }

    inline void Inter::pushActivation(Activation *new_activation) {
        activation = new_activation;
    }

    inline Inter::InterStatus Inter::getStatus() const {
        return status;
    }

    inline bool Inter::isInterStop() const {
        return (status == inter_exit || status == inter_stop);
    }

    inline bool Inter::isInterExit() const {
        return (status == inter_exit);
    }

    inline ProtectVarSpace *Inter::getProtectVarSpace() const {
        return env.protect;
    }

    inline VarSpace *Inter::getGlobalVarSpace() const {
        return env.global;
    }

    inline VarList *Inter::getGlobalVarlist() const {
        return env.global_varlist;
    }

    inline Activation *Inter::getActivation() const {
        return activation;
    }

    inline EnvVarSpace &Inter::getEnvVarSpace() {
        return env.envvar;
    }

    inline InterMessage &Inter::getOutMessageStream() {
        return out;
    }

    inline InterMessage &Inter::getInMessageStream() {
        return in;
    }

    inline size_t Environment::operator++(){
        return ++reference;
    }

    inline size_t Environment::operator--(){
        return --reference;
    }

    inline size_t Environment::operator++(int){
        return reference++;
    }

    inline size_t Environment::operator--(int){
        return reference--;
    }

    inline Inter::InterStatus Inter::setInterStop() {
        InterStatus ret = status;
        if (status != inter_exit)
            status = inter_stop;
        return ret;
    }

    inline Inter::InterStatus Inter::setInterExit() {
        InterStatus ret = status;
        status = inter_exit;
        return ret;
    }
}

#endif //AFUN_INTER_INLINE_H
