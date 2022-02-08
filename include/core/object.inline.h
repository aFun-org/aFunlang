#ifndef AFUN_OBJECT_INLINE_H
#define AFUN_OBJECT_INLINE_H
#include "object.h"

namespace aFuncore {
    void Object::addReference() {
        std::unique_lock<std::mutex> mutex{lock};
        reference++;
    }

    void Object::delReference() {
        std::unique_lock<std::mutex> mutex{lock};
        reference--;
    }

    GcCount Object::getReference() const {
        return reference;
    }

    void Object::setClear(bool clear) {
        not_clear=!clear;
    }
}

#endif //AFUN_OBJECT_INLINE_H
