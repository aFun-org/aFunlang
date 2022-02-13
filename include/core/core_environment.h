#ifndef AFUN_CORE_ENVIRONMENT_H
#define AFUN_CORE_ENVIRONMENT_H
#include <list>
#include <mutex>
#include "aFunCoreExport.h"
#include "aFuntool.h"
#include "core_env_var.h"
#include "core_activation.h"
#include "core_object.h"

namespace aFuncore {
    class Inter;

    class AFUN_CORE_EXPORT Environment {
        friend class Object;
        friend class Inter;

    public:
        explicit Environment(int argc = 0, char **argv = nullptr);
        ~Environment() noexcept(false);
        Environment(Environment &) = delete;
        Environment &operator=(Environment &) = delete;

        AFUN_INLINE size_t operator++();
        AFUN_INLINE size_t operator--();
        AFUN_INLINE size_t operator++(int);
        AFUN_INLINE size_t operator--(int);

        [[nodiscard]] AFUN_INLINE EnvVarSpace &getEnvVarSpace();
    private:
        std::mutex lock;
        size_t reference;  // 引用计数
        bool destruct;
        std::list<Object *> gc;
        Inter &gc_inter;  /* 需要在lock和reference后初始化 */
        std::thread gc_thread;
        void gcThread();

    protected:  // 位于 mutex 之下
        EnvVarSpace &env_var;
    };
}


#include "core_environment.inline.h"
#endif //AFUN_CORE_ENVIRONMENT_H
