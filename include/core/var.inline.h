#ifndef AFUN_VAR_INLINE_H
#define AFUN_VAR_INLINE_H
#include "var.h"

namespace aFuncore {
    inline Object *Var::getData() {
        std::unique_lock<std::mutex> mutex{lock};
        return data;
    }

    inline void Var::setData(Object *data_) {
        std::unique_lock<std::mutex> mutex{lock};
        data = data_;
    }

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

    inline void VarList::GcLinkObject(std::queue<GcObjectBase *> &queue) {
        for (auto var : varspace)
            queue.push(var);
    }
}

#endif //AFUN_VAR_INLINE_H
