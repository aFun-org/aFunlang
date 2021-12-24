#ifndef AFUN_GC_HPP
#define AFUN_GC_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"
#include "queue"

namespace aFuncore {
    typedef unsigned GcCount;

    class GcObjectBase {
        bool not_clear;  // 不清除
        bool reachable;  // 可达标记 [同时标识已迭代]
        GcCount reference;  // 引用计数
    public:
        GcObjectBase() : not_clear{false}, reference{0}, reachable{false} {}
        void addReference() {reference++;}
        void delReference() {reference--;}
        void setClear(bool clear=false) {not_clear=!clear;}
        void setReachable(bool is_reference=false) {reachable=is_reference;}
    };

    template <class T>
    class GcObject : public GcObjectBase {
        T *prev;
        T *next;
    public:
        GcObject();
        virtual ~GcObject()=default;
        void addObject(T *&chain);
        void delObject(T *&chain);
        static void destruct(T *&chain);
    };

    class GcList {
        std::queue<GcObjectBase *> queue;
    public :
        AFUN_CORE_EXPORT size_t add(GcObjectBase *obj);
        AFUN_CORE_EXPORT GcObjectBase *pop();

        [[nodiscard]] size_t getSize() const {return queue.size();}
        [[nodiscard]] size_t isEmpty() const {return queue.empty();}
    };

};


#endif //AFUN_GC_HPP
