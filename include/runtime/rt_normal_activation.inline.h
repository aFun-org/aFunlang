#ifndef AFUN_RT_NORMAL_ACTIVATION_INLINE_H
#define AFUN_RT_NORMAL_ACTIVATION_INLINE_H

#include "rt_normal_activation.h"

namespace aFunrt {
    NormalActivation::VarList &NormalActivation::getVarlist(){
        return varlist;
    }

    NormalActivation::VarList::VarList() : varspace{} {

    }

    NormalActivation::VarList::VarList(VarList &&new_varlist) noexcept : varspace{std::move(new_varlist.varspace)} {

    }

    NormalActivation::VarList &NormalActivation::VarList::operator=(VarList &&new_varlist)  noexcept {
        clear();
        varspace = std::move(new_varlist.varspace);
        return *this;
    }

    void NormalActivation::VarList::push(VarSpace *varspace_) {
        varspace_->addReference();
        varspace.push_front(varspace_);
    }

    size_t NormalActivation::VarList::count() {
        return varspace.size();
    }

    aFuncore::Object *NormalActivation::VarList::findObject(const std::string &name) {
        Var *var = findVar(name);
        return var ? var->getData() : nullptr;
    }
}


#endif //AFUN_RT_NORMAL_ACTIVATION_INLINE_H
