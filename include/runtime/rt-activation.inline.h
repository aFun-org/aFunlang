#ifndef AFUN_RT_ACTIVATION_INLINE_H
#define AFUN_RT_ACTIVATION_INLINE_H

#include "rt-activation.h"

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

    ExeActivation::ExeActivation(const aFuncode::Code &code, aFuncore::Inter &inter_) : NormalActivation(inter_), start{code.getByteCode()}, next{code.getByteCode()} {

    }

    ExeActivation::ExeActivation(const aFuncode::Code::ByteCode *code, aFuncore::Inter &inter_) : NormalActivation(inter_), start{code}, next{code} {

    }

    const aFuncode::Code::ByteCode *ExeActivation::getStart() const{
        return start;
    }

    const aFuncode::Code &TopActivation::getBase() const {
        return base;
    }

    FuncActivation::FuncActivation(const aFuncode::Code::ByteCode *code, aFuncore::Inter &inter_) : NormalActivation(inter_), call{code} {

    }
}


#endif //AFUN_RT_ACTIVATION_INLINE_H
