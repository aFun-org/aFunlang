#ifndef AFUN_ENV_VAR_HPP
#define AFUN_ENV_VAR_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"

namespace aFuncore {
    static const size_t ENV_VAR_HASH_SIZE = 100;  // 环境变量哈希表大小
    class EnvVarSpace {  // 环境变量
        friend class Inter;

        struct EnvVar {  // 环境变量
            std::string name;
            std::string str;
            int32_t num = 0;  // 可以同时记录字符串和数字
            struct EnvVar *next = nullptr;
        };

        size_t count;
        EnvVar *var[ENV_VAR_HASH_SIZE] {};
        pthread_rwlock_t lock;

        EnvVar *findVar(const std::string &name);
    public:
        AFUN_CORE_EXPORT EnvVarSpace();
        AFUN_CORE_EXPORT ~EnvVarSpace();

        [[nodiscard]] size_t getCount() const {return count;}

        AFUN_CORE_EXPORT bool findString(const std::string &name, std::string &str) const;
        AFUN_CORE_EXPORT bool findNumber(const std::string &name, int32_t &num) const;

        AFUN_CORE_EXPORT void setString(const std::string &name, const std::string &str);
        AFUN_CORE_EXPORT void setNumber(const std::string &name, int32_t num);
    };
}


#endif //AFUN_ENV_VAR_HPP
