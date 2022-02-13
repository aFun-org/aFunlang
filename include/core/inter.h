#ifndef AFUN_INTER_H
#define AFUN_INTER_H
#include <list>
#include <mutex>
#include "aFunCoreExport.h"
#include "aFuntool.h"
#include "aFuncode.h"
#include "env-var.h"
#include "core-message-stream.h"
#include "core-activation.h"
#include "object.h"
#include "environment.h"

namespace aFuncore {
    class Inter;

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

        explicit Inter(Environment &env_);
        Inter(const Inter &base_inter);
        ~Inter();
        Inter &operator=(const Inter &) = delete;

        void enable();

        [[nodiscard]] AFUN_INLINE InterStatus getStatus() const;
        [[nodiscard]] AFUN_INLINE bool isInterStop() const;
        [[nodiscard]] AFUN_INLINE bool isInterExit() const;
        [[nodiscard]] AFUN_INLINE Environment &getEnvironment();
        [[nodiscard]] AFUN_INLINE const std::list<Activation *> &getStack() const;
        [[nodiscard]] AFUN_INLINE Activation *getActivation() const;
        [[nodiscard]] bool checkLiteral(const std::string &element) const;
        [[nodiscard]] bool checkLiteral(const std::string &element, std::string &literaler, bool &in_protect) const;
        [[nodiscard]] AFUN_INLINE EnvVarSpace &getEnvVarSpace();
        [[nodiscard]] AFUN_INLINE InterOutMessageStream &getOutMessageStream();
        [[nodiscard]] AFUN_INLINE InterInMessageStream &getInMessageStream();

        bool pushLiteral(const std::string &pattern, const std::string &literaler, bool in_protect);

        AFUN_INLINE void pushActivation(Activation *new_activation);
        AFUN_INLINE Activation *popActivation();

        bool runCode();

        AFUN_INLINE InterStatus setInterStop();
        AFUN_INLINE InterStatus setInterExit();
    private:
        InterStatus status;

        Environment &env;

        std::list<Activation *> stack;
        Activation *activation;  // 活动记录

        InterOutMessageStream out;
        InterInMessageStream in;

        std::list<LiteralRegex> literal;
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
