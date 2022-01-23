﻿#ifndef AFUN_INTER_H
#define AFUN_INTER_H
#include <list>
#include "aFuntool.h"
#include "aFunCoreExport.h"

#include "code.h"
#include "env-var.h"
#include "msg.h"

namespace aFuncore {
    class Activation;
    class Var;
    class ProtectVarSpace;
    class VarSpace;
    class VarList;
    class Object;

    class AFUN_CORE_EXPORT Environment {
        friend class Object;
        friend class Var;
        friend class VarSpace;
        friend class Inter;

    public:
        Environment();
        ~Environment() noexcept(false);
        Environment(Environment &) = delete;
        Environment &operator=(Environment &) = delete;

        inline size_t operator++();
        inline size_t operator--();
        inline size_t operator++(int);
        inline size_t operator--(int);

    private:
        Object *obj;
        Var *var;
        VarSpace *varspace;

        ProtectVarSpace *protect;  // 保护变量空间
        VarSpace *global;  // 全局变量空间
        VarList *global_varlist;  // global + protect
        EnvVarSpace envvar;

        size_t reference;  // 引用计数
    };

    class AFUN_CORE_EXPORT Inter {
        friend class Activation;

        struct LiteralRegex;
    public:
        typedef enum InterStatus {
            inter_creat = 0,
            inter_init = 1,  // 执行初始化程序
            inter_normal = 2,  // 正常执行
            inter_stop = 3,  // 当前运算退出
            inter_exit = 4,  // 解释器退出
        } InterStatus;

        typedef enum ExitFlat {
            ef_activity = 0,  // 主动退出
            ef_passive = 1,  // 被动退出
            ef_none = 2,
        } ExitFlat;

        typedef enum ExitMode {
            em_activity = ef_activity,  // 主动退出
            em_passive = ef_passive,  // 被动退出
        } ExitMode;

        typedef enum Prefix {
            prefix_quote = 0,  // 变量引用
            prefix_exec_first = 1,
        } Prefix;

        static const int PREFIX_COUNT = 2;
        constexpr static const char *E_PREFIX = "$`'";  /* NOLINT element前缀 */
        constexpr static const char *B_PREFIX = "$`'%^&<?>";  /* NOLINT block前缀 */

        explicit Inter(Environment &env_, int argc = 0, char **argv = nullptr, ExitMode em = em_activity);
        Inter(const Inter &base_inter, ExitMode em = em_activity);
        ~Inter();
        Inter &operator=(const Inter &) = delete;

        void enable();

        [[nodiscard]] inline InterStatus getStatus() const;
        [[nodiscard]] inline bool isExit() const;
        [[nodiscard]] inline Environment &getEnvironment();
        [[nodiscard]] inline ProtectVarSpace *getProtectVarSpace() const;
        [[nodiscard]] inline VarSpace *getGlobalVarSpace() const;
        [[nodiscard]] inline VarList *getGlobalVarlist() const;
        [[nodiscard]] inline Activation *getActivation() const;
        [[nodiscard]] bool checkLiteral(const std::string &element) const;
        [[nodiscard]] bool checkLiteral(const std::string &element, std::string &literaler, bool &in_protect) const;
        [[nodiscard]] inline EnvVarSpace &getEnvVarSpace();

        bool pushLiteral(const std::string &pattern, const std::string &literaler, bool in_protect);

        bool runCode();
        bool runCode(Code *code);

    private:
        InterStatus status;

        Environment &env;

        Activation *activation;  // 活动记录

        InterMessage out;
        InterMessage in;

        std::list<LiteralRegex> literal;

        Object *result;  // 线程执行的结果

        ExitFlat exit_flat;  // 外部设置退出
        ExitMode exit_mode;  // 退出模式

        inline void pushActivation(Activation *new_activation);
    };

    struct Inter::LiteralRegex {
        aFuntool::Regex rg;
        std::string pattern;  // 派生 LiteralRegex 时使用
        std::string literaler;  // 调用的函数
        bool in_protect;  // 是否在protect空间
    };
}

#include "inter.inline.h"

#endif //AFUN_INTER_H
