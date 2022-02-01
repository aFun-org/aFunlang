﻿#ifndef AFUN_OBJECT_VALUE_INLINE_H
#define AFUN_OBJECT_VALUE_INLINE_H
#include "object-value.h"

namespace aFuncore {
    inline size_t VarSpace::getCount() {
        std::unique_lock<std::mutex> mutex{lock};
        return var.size();
    }

    inline Object *VarSpace::findObject(const std::string &name) {
        Var *ret = findVar(name);
        return ret ? ret->getData() : nullptr;
    }

    inline ProtectVarSpace::ProtectVarSpace(Inter &inter) : VarSpace(inter), is_protect{false} {

    }

    inline ProtectVarSpace::ProtectVarSpace(Environment &env_) : VarSpace(env_), is_protect{false} {

    }

    inline bool ProtectVarSpace::getProtect() const {
        return is_protect;
    }

    inline bool ProtectVarSpace::setProtect(bool protect) {
        bool ret = is_protect; is_protect = protect; return ret;
    }
};

#endif //AFUN_OBJECT_VALUE_INLINE_H