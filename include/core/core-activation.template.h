#ifndef AFUN_CORE_ACTIVATION_TEMPLATE_H
#define AFUN_CORE_ACTIVATION_TEMPLATE_H

#include "core-activation.h"

namespace aFuncore {
    template <typename Callable, typename...T>
    void Activation::VarList::forEach(Callable func, T...arg) {
        for (auto &vs : varspace)
            func(vs, arg...);
    }
}

#endif //AFUN_CORE_ACTIVATION_TEMPLATE_H
