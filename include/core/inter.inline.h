#ifndef AFUN_INTER_INLINE_H
#define AFUN_INTER_INLINE_H
#include "inter.h"

namespace aFuncore {
    Environment &Inter::getEnvironment() {
        return env;
    }

    void Inter::pushActivation(Activation *new_activation) {
        stack.push_front(new_activation);
        activation = new_activation;
    }

    Activation *Inter::popActivation() {
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

    Inter::InterStatus Inter::getStatus() const {
        return status;
    }

    bool Inter::isInterStop() const {
        return (status == inter_exit || status == inter_stop);
    }

    bool Inter::isInterExit() const {
        return (status == inter_exit);
    }

    ProtectVarSpace *Inter::getProtectVarSpace() const {
        return env.protect;
    }

    const std::list<Activation *> &Inter::getStack() const {
        return stack;
    }

    Activation *Inter::getActivation() const {
        return activation;
    }

    EnvVarSpace &Inter::getEnvVarSpace() {
        return env.envvar;
    }

    InterOutMessage &Inter::getOutMessageStream() {
        return out;
    }

    InterInMessage &Inter::getInMessageStream() {
        return in;
    }

    size_t Environment::operator++(){
        std::unique_lock<std::mutex> mutex{lock};
        return ++reference;
    }

    size_t Environment::operator--(){
        std::unique_lock<std::mutex> mutex{lock};
        return --reference;
    }

    size_t Environment::operator++(int){
        std::unique_lock<std::mutex> mutex{lock};
        return reference++;
    }

    size_t Environment::operator--(int){
        std::unique_lock<std::mutex> mutex{lock};
        return reference--;
    }

    Inter::InterStatus Inter::setInterStop() {
        InterStatus ret = status;
        if (status != inter_exit)
            status = inter_stop;
        return ret;
    }

    Inter::InterStatus Inter::setInterExit() {
        InterStatus ret = status;
        status = inter_exit;
        return ret;
    }
}

#endif //AFUN_INTER_INLINE_H
