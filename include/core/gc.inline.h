#ifndef AFUN_GC_INLINE_H
#define AFUN_GC_INLINE_H
#include "gc.h"

namespace aFuncore {
    inline GcObjectBase::GcObjectBase() : not_clear{false}, reference{0}, reachable{false} {

    }

    inline void GcObjectBase::addReference() {
        reference++;
    }

    inline void GcObjectBase::delReference() {
        reference--;
    }

    inline void GcObjectBase::setClear(bool clear) {
        not_clear=!clear;
    }

    inline void GcObjectBase::setReachable(bool is_reference) {
        reachable=is_reference;
    }


    inline size_t GcList::getSize() const {
        return queue.size();
    }

    inline size_t GcList::isEmpty() const {
        return queue.empty();
    }
}

#endif //AFUN_GC_INLINE_H
