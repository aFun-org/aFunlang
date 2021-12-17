#ifndef AFUN_GC_HPP_
#define AFUN_GC_HPP_
#include "gc.hpp"

namespace aFuncore {
    template<class T>
    GcObject<T>::GcObject() : GcObjectBase() {
        prev = nullptr;
        next = nullptr;
    }

    template<class T>
    void aFuncore::GcObject<T>::addObject(T *&chain){
        if (chain != nullptr) {
            next = chain;
            chain->prev = dynamic_cast<T *>(this);
        }
        chain = dynamic_cast<T *>(this);
    }

    template<class T>
    void aFuncore::GcObject<T>::delObject(T *&chain){
        if (next != nullptr)
            next->prev = prev;

        if (prev == nullptr)
            chain = next;
        else
            prev->next = next;
    }

    template<class T>
    void GcObject<T>::destruct(T *&chain){
        for (T *tmp = chain, *n; tmp != nullptr; tmp = n) {
            n = tmp->next;
            delete tmp;
        }
    }

};

#endif //AFUN_GC_HPP_
