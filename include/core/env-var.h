#ifndef AFUN_ENV_VAR_H
#define AFUN_ENV_VAR_H
#include <unordered_map>
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "shared_mutex"

namespace aFuncore {
    class AFUN_CORE_EXPORT EnvVarSpace {  // 环境变量
    public:
        EnvVarSpace() = default;
        ~EnvVarSpace() = default;
        EnvVarSpace(const EnvVarSpace &)=delete;
        EnvVarSpace &operator=(const EnvVarSpace &)=delete;

        [[nodiscard]] inline size_t getCount() const;
        bool findString(const std::string &name, std::string &str) const;
        bool findNumber(const std::string &name, int32_t &num) const;

        void setString(const std::string &name, const std::string &str);
        void setNumber(const std::string &name, int32_t num);

        void addString(const std::string &name, const std::string &str);
        void addNumber(const std::string &name, int32_t num);

    private:
        static const size_t ENV_VAR_HASH_SIZE = 100;  // 环境变量哈希表大小
        struct EnvVar {  // 环境变量
            std::string str;
            int32_t num;  // 可以同时记录字符串和数字
        };

        std::unordered_map<std::string, EnvVar> var;
        std::shared_mutex lock;
    };
}

#include "env-var.inline.h"

#endif //AFUN_ENV_VAR_H
