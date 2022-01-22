#ifndef AFUN_INTER_H
#define AFUN_INTER_H
#include <list>
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "core.h"

#include "value.h"
#include "var.h"
#include "activation.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Inter {
        /* 解释器原信息记录 */
        InterStatus status;

        /* GC 记录器 */
        struct GcRecord {
            Object *obj;
            Var *var;
            VarSpace *varspace;
        } *gc;
        [[nodiscard]] inline struct GcRecord *getGcRecord() const;
        friend Object::Object(const std::string &type_, Inter &inter_);
        friend Var::Var(Object *data_, Inter &inter_);
        friend VarSpace::VarSpace(Inter &inter_);

        /* 运行相关 */
        ProtectVarSpace *protect;  // 保护变量空间
        VarSpace *global;  // 全局变量空间
        VarList *global_varlist;  // global + protect
        Activation *activation;  // 活动记录
        InterMessage out;
        InterMessage in;

        inline void pushActivation(Activation *new_activation);
        friend Activation::Activation(Inter &inter_);

        struct LiteralRegex {
            Regex rg;
            std::string pattern;  // 派生 LiteralRegex 时使用
            std::string literaler;  // 调用的函数
            bool in_protect;  // 是否在protect空间
        };
        std::list<LiteralRegex> *literal;

        /* 配置信息记录器 */
        EnvVarSpace &envvar;

        /* 线程信息 */
    public:
        const bool is_derive;  // 是否派生
        Inter &base;  // 主线程
    private:
        Object *result;  // 线程执行的结果
        std::list<Inter *> *son_inter;  // 派生线程链表, 由主线程负责管理

        ExitFlat exit_flat;  // 外部设置退出
        ExitMode exit_mode;  // 退出模式
    public:
        explicit Inter(int argc=0, char **argv=nullptr, ExitMode em=em_activity);
        ~Inter();
        Inter(const Inter &)=delete;
        Inter &operator=(const Inter &)=delete;

        void enable();

        [[nodiscard]] inline InterStatus getStatus() const;
        [[nodiscard]] inline bool isExit() const;

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
    };
}

#include "inter.inline.h"

#endif //AFUN_INTER_H
