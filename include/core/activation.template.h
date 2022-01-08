#ifndef AFUN_ACTIVATION_TEMPLATE_H
#define AFUN_ACTIVATION_TEMPLATE_H
#include "activation.h"

namespace aFuncore {
    template<typename T>
    void Activation::forEach(Activation *activation, void (*func)(Activation *activation, Message *, T), T arg){
        for (NULL; activation != nullptr; activation = activation->prev)
            func(activation, arg);
    }
}

#endif //AFUN_ACTIVATION_TEMPLATE_H
