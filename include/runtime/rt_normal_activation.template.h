#ifndef AFUN_RT_NORMAL_ACTIVATION_TEMPLATE_H
#define AFUN_RT_NORMAL_ACTIVATION_TEMPLATE_H
#include "rt_normal_activation.h"

namespace aFunrt {
    template <typename Callable, typename...T>
    void NormalActivation::VarList::forEach(Callable func, T...arg) {
        for (auto &vs : varspace)
            func(vs, arg...);
    }
}


#endif //AFUN_RT_NORMAL_ACTIVATION_TEMPLATE_H
