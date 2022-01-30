﻿#ifndef AFUN_GC_H
#define AFUN_GC_H
#include <queue>
#include <list>
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    typedef unsigned GcCount;
    class Inter;

    class AFUN_CORE_EXPORT GcObjectBase {
    public:
        GcObjectBase(const GcObjectBase &) = delete;
        GcObjectBase &operator=(const GcObjectBase &) = delete;

        inline void addReference();
        inline void delReference();
        [[nodiscard]] inline GcCount getReference() const;
        inline void setClear(bool clear=false);
        inline void setReachable(bool is_reference=false);

        static void destructAll(std::list<GcObjectBase *> &list, Inter &gc_inter);
    protected:
        std::mutex lock;

        inline GcObjectBase();
        virtual ~GcObjectBase() = default;

    private:
        bool not_clear;  // 不清除
        bool reachable;  // 可达标记 [同时标识已迭代]
        GcCount reference;  // 引用计数
    };


    class AFUN_CORE_EXPORT GcList {
    public :
        size_t add(GcObjectBase *obj);
        GcObjectBase *pop();

        [[nodiscard]] inline size_t getSize() const;
        [[nodiscard]] inline size_t isEmpty() const;

    private:
        std::queue<GcObjectBase *> queue;
    };

};

#include "gc.inline.h"

#endif //AFUN_GC_H
