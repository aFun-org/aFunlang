#ifndef AFUN_GC_TEMPLATE_H
#define AFUN_GC_TEMPLATE_H

#include "gc.h"

namespace aFuncore {
    template<class T>
    class GcObject : public GcObjectBase {
        T *prev;
        T *next;
    protected:
        GcObject() : GcObjectBase(), prev{nullptr}, next{nullptr}{}

    public:
        void addObject(T *&chain){
            if (chain != nullptr) {
                next = chain;
                chain->prev = dynamic_cast<T *>(this);
            }
            chain = dynamic_cast<T *>(this);
        }

        void delObject(T *&chain){
            if (next != nullptr)
                next->prev = prev;

            if (prev == nullptr)
                chain = next;
            else
                prev->next = next;
        }

        static void destruct(T *&chain){
            for (T *tmp = chain, *n; tmp != nullptr; tmp = n) {
                n = tmp->next;
                delete tmp;
            }
        }
    };
}

#endif //AFUN_GC_TEMPLATE_H
