#ifndef AFUN_CORE_ACTIVATION_INLINE_H
#define AFUN_CORE_ACTIVATION_INLINE_H

#include "core-activation.h"

namespace aFuncore {
    inline Activation::VarList &Activation::getVarlist(){
        return varlist;
    }

    inline UpMessage &Activation::getUpStream() {
        return up;
    }

    inline DownMessage &Activation::getDownStream() {
        return down;
    }

    inline aFuntool::FileLine Activation::getFileLine() const{
        return line;
    }

    inline const aFuntool::FilePath &Activation::getFilePath() const{
        return path;
    }

    inline Activation::VarList::VarList() : varspace{} {

    }

    inline Activation::VarList::VarList(VarList &&new_varlist) : varspace{std::move(new_varlist.varspace)} {

    }

    inline Activation::VarList &Activation::VarList::operator=(VarList &&new_varlist) {
        clear();
        varspace = std::move(new_varlist.varspace);
        return *this;
    }

    inline void Activation::VarList::push(VarSpace *varspace_) {
        varspace_->addReference();
        varspace.push_front(varspace_);
    }

    inline size_t Activation::VarList::count() {
        return varspace.size();
    }

    inline Object *Activation::VarList::findObject(const std::string &name) {
        Var *var = findVar(name);
        return var ? var->getData() : nullptr;
    }

    inline ExeActivation::ExeActivation(const Code &code, Inter &inter_) : Activation(inter_), start{code.getByteCode()}, next{code.getByteCode()} {

    }

    inline ExeActivation::ExeActivation(const Code::ByteCode *code, Inter &inter_) : Activation(inter_), start{code}, next{code} {

    }

    inline const Code::ByteCode *ExeActivation::getStart() const{
        return start;
    }

    const Code &TopActivation::getBase() const {
        return base;
    }

    inline FuncActivation::FuncActivation(const Code::ByteCode *code, Inter &inter_) : Activation(inter_), call{code} {

    }
}

#endif //AFUN_CORE_ACTIVATION_INLINE_H
