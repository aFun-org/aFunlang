#ifndef AFUN_CORE_ACTIVATION_INLINE_H
#define AFUN_CORE_ACTIVATION_INLINE_H

#include "core-activation.h"

namespace aFuncore {
    inline void Activation::endRun() {

    }

    inline VarList *Activation::getVarlist() const{
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
