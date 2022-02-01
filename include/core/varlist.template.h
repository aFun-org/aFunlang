#ifndef AFUN_VARLIST_TEMPLATE_H
#define AFUN_VARLIST_TEMPLATE_H

#include "varlist.h"

namespace aFuncore {
    template <typename Callable, typename...T>
    void VarList::forEach(Callable func, T...arg) {
        std::unique_lock<std::mutex> mutex{lock};
        for (auto &vs : varspace) {
            mutex.unlock();
            func(vs, arg...);
            mutex.lock();
        }
    }

    template <typename Callable, typename...T>
    void VarList::forEachLock(Callable func, T...arg) {
        std::unique_lock<std::mutex> mutex{lock};
        for (auto &vs : varspace)
            func(vs, arg...);
    }
}

#endif //AFUN_VARLIST_TEMPLATE_H
