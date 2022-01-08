#ifndef AFUN_ENV_VAR_H
#define AFUN_ENV_VAR_H
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT EnvVarSpace {  // 环境变量
        static const size_t ENV_VAR_HASH_SIZE = 100;  // 环境变量哈希表大小
        struct EnvVar {  // 环境变量
            std::string name;
            std::string str;
            int32_t num = 0;  // 可以同时记录字符串和数字
            struct EnvVar *next = nullptr;
        };

        size_t count;
        EnvVar *var[ENV_VAR_HASH_SIZE] {};
        pthread_rwlock_t lock;
    public:
        EnvVarSpace();
        ~EnvVarSpace();
        EnvVarSpace(const EnvVarSpace &)=delete;
        EnvVarSpace &operator=(const EnvVarSpace &)=delete;

        [[nodiscard]] size_t getCount() const;
        bool findString(const std::string &name, std::string &str) const;
        bool findNumber(const std::string &name, int32_t &num) const;

        void setString(const std::string &name, const std::string &str);
        void setNumber(const std::string &name, int32_t num);

        void addString(const std::string &name, const std::string &str);
        void addNumber(const std::string &name, int32_t num);
    };
}

#include "env-var.inline.h"

#endif //AFUN_ENV_VAR_H
