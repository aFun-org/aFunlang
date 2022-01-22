#ifndef AFUN_GC_H
#define AFUN_GC_H
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "queue"

namespace aFuncore {
    typedef unsigned GcCount;

    class AFUN_CORE_EXPORT GcObjectBase {
        bool not_clear;  // 不清除
        bool reachable;  // 可达标记 [同时标识已迭代]
        GcCount reference;  // 引用计数
    protected:
        inline GcObjectBase();
        virtual ~GcObjectBase() = default;
    public:
        GcObjectBase(const GcObjectBase &) = delete;
        GcObjectBase &operator=(const GcObjectBase &) = delete;

        inline void addReference();
        inline void delReference();
        inline void setClear(bool clear=false);
        inline void setReachable(bool is_reference=false);
    };


    class AFUN_CORE_EXPORT GcList {
        std::queue<GcObjectBase *> queue;
    public :
        size_t add(GcObjectBase *obj);
        GcObjectBase *pop();

        [[nodiscard]] inline size_t getSize() const;
        [[nodiscard]] inline size_t isEmpty() const;
    };

};

#include "gc.inline.h"
#include "gc.template.h"

#endif //AFUN_GC_H
