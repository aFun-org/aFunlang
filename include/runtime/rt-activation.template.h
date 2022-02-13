#ifndef AFUN_RT_ACTIVATION_TEMPLATE_H
#define AFUN_RT_ACTIVATION_TEMPLATE_H
#include "rt-activation.h"

namespace aFunrt {
    template <typename Callable, typename...T>
    void NormalActivation::VarList::forEach(Callable func, T...arg) {
        for (auto &vs : varspace)
            func(vs, arg...);
    }
}


#endif //AFUN_RT_ACTIVATION_TEMPLATE_H
