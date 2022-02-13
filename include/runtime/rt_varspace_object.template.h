#ifndef AFUN_RT_VARSPACE_OBJECT_TEMPLATE_H
#define AFUN_RT_VARSPACE_OBJECT_TEMPLATE_H

#include "rt_varspace_object.h"

namespace aFunrt {
    template <typename Callable, typename...T>
    void VarSpace::forEach(Callable func, T...arg) {
        std::unique_lock<std::mutex> mutex{lock};
        for (auto &tmp : var) {
            mutex.unlock();
            func(tmp.second, arg...);
            mutex.lock();
        }
    }

    template <typename Callable, typename...T>
    void VarSpace::forEachLock(Callable func, T...arg) {
        std::unique_lock<std::mutex> mutex{lock};
        for (auto &tmp : var)
            func(tmp.second, arg...);
    }
}

#endif //AFUN_RT_VARSPACE_OBJECT_TEMPLATE_H
