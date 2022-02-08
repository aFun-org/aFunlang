#include "object.h"
#include "inter.h"
#include "core-init.h"

namespace aFuncore {
    Object::Object(std::string type_, Inter &inter)
        : env{inter.getEnvironment()}, type{std::move(type_)}, done_destruct{false}, not_clear{false},
          reachable{false}, reference{1}{
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.push_back(this);
    }

    Object::Object(std::string type_, Environment &env_)
        : env{env_}, type{std::move(type_)}, done_destruct{false}, not_clear{false},
          reachable{false}, reference{1}{
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.push_back(this);
    }

    Object::~Object() {
        if (reference != 0)
            warningLog(aFunCoreLogger, "Object %p destruct reference: %d", this, getReference());
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.remove(this);
    }

    void Object::destructAll(std::list<Object *>& list, Inter &gc_inter) {
        for (auto obj : list)
            if (!obj->done_destruct) {
                obj->done_destruct = true;
                obj->destruct(gc_inter);
            }
    }

    void Object::deleteAll(std::list<Object *>& list) {
        while (!list.empty())
            delete list.front();  /* 自带pop */
        if (!list.empty())
            warningLog(aFunCoreLogger, "After Object destructAll, list is not empty");
    }

    void Object::destruct(Inter &inter) {
        /* 什么都不做, 但virtual函数不能是inline */
    }

    void Object::linkObject(std::queue<Object *> &queue) {
        /* 什么都不做, 但virtual函数不能是inline */
    }

    void Object::checkReachable(std::list<Object *> &list) {
        std::queue<Object *> queue;

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

    void Object::setReachable(std::list<Object *> &list, std::queue<Object *> &des,
                                    std::queue<Object *> &del) {
        for (auto *obj : list) {
            if (!obj->reachable) {
                if (obj->done_destruct)
                    del.push(obj);
                else
                    des.push(obj);
            }
        }
    }

    void Object::destructUnreachable(std::queue<Object *> &des, Inter &gc_inter) {
        while (!des.empty()) {
            auto obj = des.front();
            obj->done_destruct = true;
            obj->destruct(gc_inter);
            des.pop();
        }
    }

    void Object::deleteUnreachable(std::queue<Object *> &del) {
        while (!del.empty()) {
            auto obj = del.front();
            del.pop();
            delete obj;
        }
    }
}