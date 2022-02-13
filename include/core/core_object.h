#ifndef AFUN_CORE_OBJECT_H
#define AFUN_CORE_OBJECT_H
#include <queue>
#include <list>
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    typedef unsigned GcCount;
    class Inter;
    class Environment;

    class AFUN_CORE_EXPORT Object {
    public:
        Object(std::string type_, Inter &inter);
        Object(std::string type_, Environment &env_);
        virtual ~Object();
        Object(const Object &) = delete;
        Object &operator=(const Object &) = delete;

        AFUN_INLINE void addReference();
        AFUN_INLINE void delReference();
        [[nodiscard]] AFUN_INLINE GcCount getReference() const;
        AFUN_INLINE void setClear(bool clear=false);

        AFUN_STATIC void checkReachable(std::list<Object *> &list);
        AFUN_STATIC void setReachable(std::list<Object *> &list, std::queue<Object *> &des, std::queue<Object *> &del);
        AFUN_STATIC void destructUnreachable(std::queue<Object *> &des, Inter &gc_inter);
        AFUN_STATIC void deleteUnreachable(std::queue<Object *> &del);
        AFUN_STATIC void destructAll(std::list<Object *> &list, Inter &gc_inter);
        AFUN_STATIC void deleteAll(std::list<Object *> &list);
    protected:
        Environment &env;
        const std::string type;  // 标识 Object 的字符串

        std::mutex lock;

        virtual void destruct(Inter &gc_inter);
        virtual void linkObject(std::queue<Object *> &queue);

    private:
        bool done_destruct;
        bool not_clear;  // 不清除
        bool reachable;  // 可达标记 [同时标识已迭代]
        GcCount reference;  // 引用计数
    };

};

#include "core_object.inline.h"

#endif //AFUN_CORE_OBJECT_H
