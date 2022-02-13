#ifndef AFUN_RT_VARSPACE_OBJECT_INLINE_H
#define AFUN_RT_VARSPACE_OBJECT_INLINE_H

#include "rt_varspace_object.h"

namespace aFunrt {
    size_t VarSpace::getCount() {
        std::unique_lock<std::mutex> mutex{lock};
        return var.size();
    }

    aFuncore::Object *VarSpace::findObject(const std::string &name) {
        Var *ret = findVar(name);
        return ret ? ret->getData() : nullptr;
    }
}


#endif //AFUN_RT_VARSPACE_OBJECT_INLINE_H
