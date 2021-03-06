#ifndef AFUN_CORE_ENV_VAR_H
#define AFUN_CORE_ENV_VAR_H
#include <unordered_map>
#include <thread>
#include <mutex>
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    class Object;

    class AFUN_CORE_EXPORT EnvVarSpace {  // 环境变量
    public:
        EnvVarSpace() = default;
        ~EnvVarSpace();
        EnvVarSpace(const EnvVarSpace &)=delete;
        EnvVarSpace &operator=(const EnvVarSpace &)=delete;

        [[nodiscard]] AFUN_INLINE size_t getCount();
        bool findString(const std::string &name, std::string &str);
        bool findNumber(const std::string &name, int32_t &num);
        bool findObject(const std::string &name, Object *&obj);

        void setString(const std::string &name, const std::string &str);
        void setNumber(const std::string &name, int32_t num);
        void setObject(const std::string &name, Object *obj);

        void addString(const std::string &name, const std::string &str);
        void addNumber(const std::string &name, int32_t num);

    private:
        AFUN_STATIC const size_t ENV_VAR_HASH_SIZE = 100;  // 环境变量哈希表大小
        struct EnvVar {  // 环境变量
            std::string str;
            int32_t num;
            Object *object;
        };

        std::unordered_map<std::string, EnvVar> var;
        std::mutex lock;
    };
}

#include "core_env_var.inline.h"

#endif //AFUN_CORE_ENV_VAR_H
