#ifndef AFUN_GC_H
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

        static void checkReachable(std::list<GcObjectBase *> &list);
        static void setReachable(std::list<GcObjectBase *> &list, std::queue<GcObjectBase *> &des, std::queue<GcObjectBase *> &del);
        static void destructUnreachable(std::queue<GcObjectBase *> &des, Inter &gc_inter);
        static void deleteUnreachable(std::queue<GcObjectBase *> &del);
        static void destructAll(std::list<GcObjectBase *> &list, Inter &gc_inter);
        static void deleteAll(std::list<GcObjectBase *> &list);
    protected:
        std::mutex lock;

        inline GcObjectBase();
        virtual ~GcObjectBase() = default;

        virtual void destruct(Inter &gc_inter);
        virtual void linkObject(std::queue<GcObjectBase *> &queue);

    private:
        bool done_destruct;
        bool not_clear;  // 不清除
        bool reachable;  // 可达标记 [同时标识已迭代]
        GcCount reference;  // 引用计数
    };

};

#include "gc.inline.h"

#endif //AFUN_GC_H
