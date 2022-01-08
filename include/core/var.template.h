#ifndef AFUN_VAR_TEMPLATE_H
#define AFUN_VAR_TEMPLATE_H

namespace aFuncore {
    template <typename Callable,typename...T>
    void VarSpace::forEach(Callable func, T...arg) {
        for (int i = 0; i < VAR_HASH_SIZE; i++) {
            for (auto tmp = var[i]; tmp != nullptr; tmp = tmp->next)
                func(tmp, arg...);
        }
    }

    template <typename Callable,typename...T>
    void VarList::forEach(Callable func, T...arg) {
        for (auto vs : varspace)
            func(vs, arg...);
    }
}

#endif //AFUN_VAR_TEMPLATE_H
