#ifndef AFUN_GC_INLINE_H
#define AFUN_GC_INLINE_H
#include "gc.h"

namespace aFuncore {
    inline GcObjectBase::GcObjectBase() : not_clear{false}, reference{1}, reachable{false} {

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
