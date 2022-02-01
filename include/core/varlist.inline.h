#ifndef AFUN_VARLIST_INLINE_H
#define AFUN_VARLIST_INLINE_H
#include "varlist.h"

namespace aFuncore {
    inline VarList::VarList(VarSpace *varspace) {
        this->varspace.push_front(varspace);
    }

    inline void VarList::push(VarSpace *varspace_) {
        std::unique_lock<std::mutex> mutex{lock};
        varspace.push_front(varspace_);
    }

    inline Object *VarList::findObject(const std::string &name) {
        Var *var = findVar(name);
        return var ? var->getData() : nullptr;
    }

    inline void VarList::GcLinkObject(std::queue<Object *> &queue) {
        for (auto var : varspace)
            queue.push(var);
    }
}

#endif //AFUN_VARLIST_INLINE_H
