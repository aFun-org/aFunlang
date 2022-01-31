#include "gc.h"
#include "inter.h"
#include "init.h"

namespace aFuncore {
    void GcObjectBase::destructAll(std::list<GcObjectBase *>& list, Inter &gc_inter) {
        for (auto obj : list)
            if (!obj->done_destruct) {
                obj->done_destruct = true;
                obj->destruct(gc_inter);
            }
    }

    void GcObjectBase::deleteAll(std::list<GcObjectBase *>& list) {
        while (!list.empty())
            delete list.front();  /* 自带pop */
        if (!list.empty())
            warningLog(aFunCoreLogger, "After GcObjectBase destructAll, list is not empty");
    }

    void GcObjectBase::destruct(Inter &inter) {
        /* 什么都不做, 但virtual函数不能是inline */
    }

    void GcObjectBase::linkObject(std::queue<GcObjectBase *> &queue) {
        /* 什么都不做, 但virtual函数不能是inline */
    }

    void GcObjectBase::checkReachable(std::list<GcObjectBase *> &list) {
        std::queue<GcObjectBase *> queue;

        for (auto *obj : list) {
            obj->reachable = false;
            if (obj->reference > 0 || obj->not_clear)
                queue.push(obj);
        }

        while (!queue.empty()) {
            auto obj = queue.front();
            if (!obj->reachable) {
                obj->reachable = true;
                obj->linkObject(queue);
            }
            queue.pop();
        }
    }

    void GcObjectBase::setReachable(std::list<GcObjectBase *> &list, std::queue<GcObjectBase *> &des,
                                    std::queue<GcObjectBase *> &del) {
        for (auto *obj : list) {
            if (!obj->reachable) {
                if (obj->done_destruct)
                    del.push(obj);
                else
                    des.push(obj);
            }
        }
    }

    void GcObjectBase::destructUnreachable(std::queue<GcObjectBase *> &des, Inter &gc_inter) {
        while (!des.empty()) {
            auto obj = des.front();
            obj->done_destruct = true;
            obj->destruct(gc_inter);
            des.pop();
        }
    }

    void GcObjectBase::deleteUnreachable(std::queue<GcObjectBase *> &del) {
        while (!del.empty()) {
            auto obj = del.front();
            del.pop();
            delete obj;
        }
    }
}