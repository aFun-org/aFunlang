#ifndef AFUN_INTER_INLINE_H
#define AFUN_INTER_INLINE_H
#include "inter.h"

namespace aFuncore {
    inline Environment &Inter::getEnvironment() {
        return env;
    }

    inline void Inter::pushActivation(Activation *new_activation) {
        stack.push_front(new_activation);
        activation = new_activation;
    }

    inline Activation *Inter::popActivation() {
        if (activation == nullptr)
            return nullptr;

        Activation *ret = activation;
        stack.pop_front();
        if (stack.empty())
            activation = nullptr;
        else
            activation = stack.front();
        return ret;
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

    inline const std::list<Activation *> &Inter::getStack() const {
        return stack;
    }

    inline Activation *Inter::getActivation() const {
        return activation;
    }

    inline EnvVarSpace &Inter::getEnvVarSpace() {
        return env.envvar;
    }

    inline InterOutMessage &Inter::getOutMessageStream() {
        return out;
    }

    inline InterInMessage &Inter::getInMessageStream() {
        return in;
    }

    inline size_t Environment::operator++(){
        std::unique_lock<std::mutex> mutex{lock};
        return ++reference;
    }

    inline size_t Environment::operator--(){
        std::unique_lock<std::mutex> mutex{lock};
        return --reference;
    }

    inline size_t Environment::operator++(int){
        std::unique_lock<std::mutex> mutex{lock};
        return reference++;
    }

    inline size_t Environment::operator--(int){
        std::unique_lock<std::mutex> mutex{lock};
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
