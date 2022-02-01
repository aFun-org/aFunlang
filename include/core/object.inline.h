#ifndef AFUN_OBJECT_INLINE_H
#define AFUN_OBJECT_INLINE_H
#include "object.h"

namespace aFuncore {
    inline void Object::addReference() {
        std::unique_lock<std::mutex> mutex{lock};
        reference++;
    }

    inline void Object::delReference() {
        std::unique_lock<std::mutex> mutex{lock};
        reference--;
    }

    inline GcCount Object::getReference() const {
        return reference;
    }

    inline void Object::setClear(bool clear) {
        not_clear=!clear;
    }
}

#endif //AFUN_OBJECT_INLINE_H
