#ifndef AFUN_ACTIVATION_INLINE_H
#define AFUN_ACTIVATION_INLINE_H

#include "activation.h"

namespace aFuncore {
    inline void Activation::endRun() {

    }

    inline VarList *Activation::getVarlist() const{
        return varlist;
    }

    inline Activation *Activation::toPrev() const{
        return prev;
    }

    inline UpMessage &Activation::getUpStream() {
        return up;
    }

    inline DownMessage &Activation::getDownStream() {
        return down;
    }

    inline FileLine Activation::getFileLine() const{
        return line;
    }

    inline const StringFilePath &Activation::getFilePath() const{
        return path;
    }

    inline ExeActivation::ExeActivation(Code *code, Inter &inter_) : Activation(inter_), start{code}, next{code}{

    }

    inline Code *ExeActivation::getStart() const{
        return start;
    }

    inline FuncActivation::FuncActivation(Code *code, Inter &inter_) : Activation(inter_), call{code,}{

    }
}

#endif //AFUN_ACTIVATION_INLINE_H
