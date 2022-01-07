#ifndef AFUN_DLC_INLINE_H
#define AFUN_DLC_INLINE_H

#include "dlc.h"

namespace aFuntool {
    template<typename SYMBOL>
    DlcSymbol<SYMBOL> *DlcHandle::get_symbol(const std::string &name){
        auto symbol = (SYMBOL *) dlsym(handle, name.c_str());
        if (symbol == nullptr)
            return nullptr;
        return new DlcSymbol<SYMBOL>(symbol, this);
    }
}

#endif //AFUN_DLC_INLINE_H
