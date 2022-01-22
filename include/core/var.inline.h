#ifndef AFUN_VAR_INLINE_H
#define AFUN_VAR_INLINE_H
#include "var.h"

namespace aFuncore {
    inline Object *Var::getData() {
        return data;
    }

    inline void Var::setData(Object *data_) {
        data = data_;
    }

    inline size_t VarSpace::getCount() const {
        return count;
    }

    inline Object *VarSpace::findObject(const std::string &name) {
        Var *ret = findVar(name);
        return ret ? ret->getData() : nullptr;
    }

    inline ProtectVarSpace::ProtectVarSpace(Inter &inter_) : VarSpace(inter_), is_protect{false} {

    }

    inline bool ProtectVarSpace::getProtect() const {
        return is_protect;
    }

    inline bool ProtectVarSpace::setProtect(bool protect) {
        bool ret = is_protect; is_protect = protect; return ret;
    }

    inline VarList::VarList(VarSpace *varspace) {
        this->varspace.push_front(varspace);
    }

    inline void VarList::push(VarSpace *varspace_) {
        varspace.push_front(varspace_);
    }

    inline Object *VarList::findObject(const std::string &name) {
        Var *var = findVar(name);
        return var ? var->getData() : nullptr;
    }
}

#endif //AFUN_VAR_INLINE_H
