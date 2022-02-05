#ifndef AFUN_INTER_H
#define AFUN_INTER_H
#include <list>
#include <mutex>
#include "aFuntool.h"
#include "aFunCoreExport.h"

#include "code.h"
#include "env-var.h"
#include "msg.h"

namespace aFuncore {
    class Activation;
    class Object;
    class Var;
    class ProtectVarSpace;
    class VarSpace;
    class Object;
    class Inter;

    class AFUN_CORE_EXPORT Environment {
        friend class Object;
        friend class Inter;

    public:
        explicit Environment(int argc = 0, char **argv = nullptr);
        ~Environment() noexcept(false);
        Environment(Environment &) = delete;
        Environment &operator=(Environment &) = delete;

        inline size_t operator++();
        inline size_t operator--();
        inline size_t operator++(int);
        inline size_t operator--(int);

    private:
        std::mutex lock;
        size_t reference;  // 引用计数
        bool destruct;
        std::list<Object *> gc;
        Inter &gc_inter;  /* 需要在lock和reference后初始化 */
        std::thread gc_thread;
        void gcThread();

    protected:  // 位于 mutex 之下
        ProtectVarSpace *const protect;  // 保护变量空间
        EnvVarSpace envvar;
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

        typedef enum Prefix {
            prefix_quote = 0,  // 变量引用
            prefix_exec_first = 1,
        } Prefix;

        static const int PREFIX_COUNT = 2;
        constexpr static const char *E_PREFIX = "$`'";  /* NOLINT element前缀 */
        constexpr static const char *B_PREFIX = "$`'%^&<?>";  /* NOLINT block前缀 */
        constexpr static const char *ALL_PREFIX = "$`'%^&<?>";  /* NOLINT block前缀 */

        explicit Inter(Environment &env_);
        Inter(const Inter &base_inter);
        ~Inter();
        Inter &operator=(const Inter &) = delete;

        void enable();

        [[nodiscard]] inline InterStatus getStatus() const;
        [[nodiscard]] inline bool isInterStop() const;
        [[nodiscard]] inline bool isInterExit() const;
        [[nodiscard]] inline Environment &getEnvironment();
        [[nodiscard]] inline ProtectVarSpace *getProtectVarSpace() const;
        [[nodiscard]] inline const std::list<Activation *> &getStack() const;
        [[nodiscard]] inline Activation *getActivation() const;
        [[nodiscard]] bool checkLiteral(const std::string &element) const;
        [[nodiscard]] bool checkLiteral(const std::string &element, std::string &literaler, bool &in_protect) const;
        [[nodiscard]] inline EnvVarSpace &getEnvVarSpace();
        [[nodiscard]] inline InterOutMessage &getOutMessageStream();
        [[nodiscard]] inline InterInMessage &getInMessageStream();

        bool pushLiteral(const std::string &pattern, const std::string &literaler, bool in_protect);

        bool runCode();
        bool runCode(const Code &code);
        bool runCode(Object *obj);

        inline InterStatus setInterStop();
        inline InterStatus setInterExit();
    private:
        InterStatus status;

        Environment &env;

        std::list<Activation *> stack;
        Activation *activation;  // 活动记录

        InterOutMessage out;
        InterInMessage in;

        std::list<LiteralRegex> literal;

        inline void pushActivation(Activation *new_activation);
        inline Activation *popActivation();
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
