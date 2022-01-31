#ifndef AFUN_GC_INLINE_H
#define AFUN_GC_INLINE_H
#include "gc.h"

namespace aFuncore {
    inline GcObjectBase::GcObjectBase() : not_clear{false}, reference{1}, reachable{false}, done_destruct{false} {

    }

    inline void GcObjectBase::addReference() {
        std::unique_lock<std::mutex> mutex{lock};
        reference++;
    }

    inline void GcObjectBase::delReference() {
        std::unique_lock<std::mutex> mutex{lock};
        reference--;
    }

    inline GcCount GcObjectBase::getReference() const {
        return reference;
    }

    inline void GcObjectBase::setClear(bool clear) {
        not_clear=!clear;
    }
}

#endif //AFUN_GC_INLINE_H
