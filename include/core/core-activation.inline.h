#ifndef AFUN_CORE_ACTIVATION_INLINE_H
#define AFUN_CORE_ACTIVATION_INLINE_H

#include "core-activation.h"

namespace aFuncore {
    Activation::VarList &Activation::getVarlist(){
        return varlist;
    }

    UpMessage &Activation::getUpStream() {
        return up;
    }

    DownMessage &Activation::getDownStream() {
        return down;
    }

    aFuntool::FileLine Activation::getFileLine() const{
        return line;
    }

    const aFuntool::FilePath &Activation::getFilePath() const{
        return path;
    }

    Activation::VarList::VarList() : varspace{} {

    }

    Activation::VarList::VarList(VarList &&new_varlist) : varspace{std::move(new_varlist.varspace)} {

    }

    Activation::VarList &Activation::VarList::operator=(VarList &&new_varlist)  noexcept {
        clear();
        varspace = std::move(new_varlist.varspace);
        return *this;
    }

    void Activation::VarList::push(VarSpace *varspace_) {
        varspace_->addReference();
        varspace.push_front(varspace_);
    }

    size_t Activation::VarList::count() {
        return varspace.size();
    }

    Object *Activation::VarList::findObject(const std::string &name) {
        Var *var = findVar(name);
        return var ? var->getData() : nullptr;
    }

    ExeActivation::ExeActivation(const Code &code, Inter &inter_) : Activation(inter_), start{code.getByteCode()}, next{code.getByteCode()} {

    }

    ExeActivation::ExeActivation(const Code::ByteCode *code, Inter &inter_) : Activation(inter_), start{code}, next{code} {

    }

    const Code::ByteCode *ExeActivation::getStart() const{
        return start;
    }

    const Code &TopActivation::getBase() const {
        return base;
    }

    FuncActivation::FuncActivation(const Code::ByteCode *code, Inter &inter_) : Activation(inter_), call{code} {

    }
}

#endif //AFUN_CORE_ACTIVATION_INLINE_H
