#ifndef AFUN_VAR_TEMPLATE_H
#define AFUN_VAR_TEMPLATE_H

namespace aFuncore {
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

#endif //AFUN_VAR_TEMPLATE_H
