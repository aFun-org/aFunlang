#ifndef AFUN_GC_HPP
#define AFUN_GC_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"
#include "queue"

namespace aFuncore {
    typedef unsigned GcCount;

    AFUN_CORE_EXPORT class GcObjectBase {
        bool not_clear;  // 不清除
        bool reachable;  // 可达标记 [同时标识已迭代]
        GcCount reference;  // 引用计数
    protected:
        GcObjectBase() : not_clear{false}, reference{0}, reachable{false} {}
        virtual ~GcObjectBase() = default;
        GcObjectBase(const GcObjectBase &)=delete;
        GcObjectBase &operator=(const GcObjectBase &)=delete;
    public:
        void addReference() {reference++;}
        void delReference() {reference--;}
        void setClear(bool clear=false) {not_clear=!clear;}
        void setReachable(bool is_reference=false) {reachable=is_reference;}
    };

    template <class T>
    class GcObject : public GcObjectBase {
        T *prev;
        T *next;
    protected:
        GcObject() : GcObjectBase(), prev {nullptr}, next {nullptr} {}
    public:
        void addObject(T *&chain) {
            if (chain != nullptr) {
                next = chain;
                chain->prev = dynamic_cast<T *>(this);
            }
            chain = dynamic_cast<T *>(this);
        }
        void delObject(T *&chain) {
            if (next != nullptr)
                next->prev = prev;

            if (prev == nullptr)
                chain = next;
            else
                prev->next = next;
        }
        static void destruct(T *&chain) {
            for (T *tmp = chain, *n; tmp != nullptr; tmp = n) {
                n = tmp->next;
                delete tmp;
            }
        }
    };

    AFUN_CORE_EXPORT class GcList {
        std::queue<GcObjectBase *> queue;
    public :
        size_t add(GcObjectBase *obj);
        GcObjectBase *pop();

        [[nodiscard]] size_t getSize() const {return queue.size();}
        [[nodiscard]] size_t isEmpty() const {return queue.empty();}
    };

};


#endif //AFUN_GC_HPP
