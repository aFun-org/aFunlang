#ifndef AFUN_INTER_H
#define AFUN_INTER_H
#include <list>
#include <mutex>
#include "aFuntool.h"
#include "aFunCoreExport.h"

#include "aFuncode.h"
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

        AFUN_INLINE size_t operator++();
        AFUN_INLINE size_t operator--();
        AFUN_INLINE size_t operator++(int);
        AFUN_INLINE size_t operator--(int);

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
            inter_init = 0,  // 执行初始化程序
            inter_normal = 1,  // 正常执行
            inter_stop = 2,  // 当前运算退出
            inter_exit = 3,  // 解释器退出
        } InterStatus;

        AFUN_STATIC const int PREFIX_COUNT = 2;  // env 记录的前缀  TODO-szh 取消

        explicit Inter(Environment &env_);
        Inter(const Inter &base_inter);
        ~Inter();
        Inter &operator=(const Inter &) = delete;

        void enable();

        [[nodiscard]] AFUN_INLINE InterStatus getStatus() const;
        [[nodiscard]] AFUN_INLINE bool isInterStop() const;
        [[nodiscard]] AFUN_INLINE bool isInterExit() const;
        [[nodiscard]] AFUN_INLINE Environment &getEnvironment();
        [[nodiscard]] AFUN_INLINE ProtectVarSpace *getProtectVarSpace() const;
        [[nodiscard]] AFUN_INLINE const std::list<Activation *> &getStack() const;
        [[nodiscard]] AFUN_INLINE Activation *getActivation() const;
        [[nodiscard]] bool checkLiteral(const std::string &element) const;
        [[nodiscard]] bool checkLiteral(const std::string &element, std::string &literaler, bool &in_protect) const;
        [[nodiscard]] AFUN_INLINE EnvVarSpace &getEnvVarSpace();
        [[nodiscard]] AFUN_INLINE InterOutMessage &getOutMessageStream();
        [[nodiscard]] AFUN_INLINE InterInMessage &getInMessageStream();

        bool pushLiteral(const std::string &pattern, const std::string &literaler, bool in_protect);

        bool runCode();
        bool runCode(const aFuncode::Code &code);
        bool runCode(Object *obj);

        AFUN_INLINE InterStatus setInterStop();
        AFUN_INLINE InterStatus setInterExit();
    private:
        InterStatus status;

        Environment &env;

        std::list<Activation *> stack;
        Activation *activation;  // 活动记录

        InterOutMessage out;
        InterInMessage in;

        std::list<LiteralRegex> literal;

        AFUN_INLINE void pushActivation(Activation *new_activation);
        AFUN_INLINE Activation *popActivation();
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
