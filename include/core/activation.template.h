#ifndef AFUN_ACTIVATION_TEMPLATE_H
#define AFUN_ACTIVATION_TEMPLATE_H
#include "activation.h"

namespace aFuncore {
    template <typename Callable, typename...T>
    inline void Activation::forEach(Activation *activation, Callable func, T...arg) {
        for (NULL; activation != nullptr; activation = activation->prev)
            func(activation, arg...);
    }
}

#endif //AFUN_ACTIVATION_TEMPLATE_H
