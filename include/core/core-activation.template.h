#ifndef AFUN_CORE_ACTIVATION_TEMPLATE_H
#define AFUN_CORE_ACTIVATION_TEMPLATE_H
#include "core-activation.h"

namespace aFuncore {
    template <typename Callable, typename...T>
    inline void Activation::forEach(Activation *activation, Callable func, T...arg) {
        for (NULL; activation != nullptr; activation = activation->prev)
            func(activation, arg...);
    }
}

#endif //AFUN_CORE_ACTIVATION_TEMPLATE_H
