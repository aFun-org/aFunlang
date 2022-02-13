#include "inter.h"
#include "object.h"
#include "environment.h"
#include "core-exception.h"

namespace aFuncore {
    void Environment::gcThread() {
        while(true) {
            std::queue<Object *> del;
            std::queue<Object *> des;
            {
                std::unique_lock<std::mutex> mutex{lock};
                if (destruct)
                    break;
                Object::checkReachable(gc);
                Object::setReachable(gc, des, del);
            }
            Object::deleteUnreachable(del);
            Object::destructUnreachable(des, gc_inter);

            int32_t intervals = 1000;
            env_var.findNumber("sys:gc-intervals", intervals);
            if (intervals < 100)
                intervals = 100;
            std::this_thread::sleep_for(std::chrono::milliseconds(intervals));
        }

        Object::destructAll(gc, gc_inter); /* 不需要mutex锁 */
    }

    Environment::Environment(int argc, char **argv)
            : reference{0}, destruct{false}, gc_inter{*(new Inter(*this))}, env_var{*new EnvVarSpace()} {
        /* 生成 gc_inter 后, reference == 1 */

        env_var.setNumber("sys:gc-intervals", 1000);
        env_var.setNumber("sys:exit-code", 0);
        env_var.setNumber("sys:argc", argc);
        for (int i = 0; i < argc; i++) {
            char buf[20];
            snprintf(buf, 10, "sys:arg%d", i);
            env_var.setString(buf, argv[i]);
        }

        gc_thread = std::thread([this](){this->gcThread();});
    }

    Environment::~Environment() noexcept(false) {
        {   /* 使用互斥锁, 防止与gc线程出现不同步的情况 */
            std::unique_lock<std::mutex> mutex{lock};
            if (reference != 1)  // gc_inter 有一个引用
                throw EnvironmentDestructException();

            if (destruct)
                return;

            destruct = true;
        }

        gc_thread.join();
        delete &gc_inter;
        delete &env_var;

        Object::deleteAll(gc); /* 不需要mutex锁 */

        if (reference != 0)
            throw EnvironmentDestructException();
    }
}