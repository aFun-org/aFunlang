#ifndef AFUN_INTER_H
#define AFUN_INTER_H
#include <list>
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "core.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Inter {
        friend class Object;
        friend class Var;
        friend class VarSpace;
        friend class Activation;

        /* 解释器原信息记录 */
        pthread_mutex_t status_lock;  // status 可能被外部使用, 因此需要用锁保护
        InterStatus status;

        /* GC 记录器 */
        struct GcRecord {
            Object *obj;
            Var *var;
            VarSpace *varspace;
        } *gc;
        [[nodiscard]] struct GcRecord *getGcRecord() const;

        /* 运行相关 */
        ProtectVarSpace *protect;  // 保护变量空间
        VarSpace *global;  // 全局变量空间
        VarList *global_varlist;  // global + protect
        Activation *activation;  // 活动记录
        void pushActivation(Activation *new_activation);

        struct LiteralRegex {
            Regex *rg;
            std::string pattern;  // 派生 LiteralRegex 时使用
            std::string literaler;  // 调用的函数
            bool in_protect;  // 是否在protect空间
        };
        std::list<LiteralRegex> *literal;

        /* 配置信息记录器 */
        EnvVarSpace *envvar;

        /* 线程信息 */
    public:
        const bool is_derive;  // 是否派生
        Inter *const base;  // 主线程
    private:
        Object *result;  // 线程执行的结果
        std::list<Inter *> *son_inter;  // 派生线程链表, 由主线程负责管理

        pthread_t monitor;  // 守护线程
        ExitFlat exit_flat;  // 外部设置退出
        ExitMode exit_mode;  // 退出模式
        pthread_mutex_t monitor_lock;
        pthread_cond_t monitor_cond;
    public:
        explicit Inter(int argc=0, char **argv=nullptr, ExitMode em=em_activity);
        ~Inter();
        Inter(const Inter &)=delete;
        Inter &operator=(const Inter &)=delete;

        void enable();

        [[nodiscard]] InterStatus getStatus() const;
        [[nodiscard]] bool isExit() const;

        [[nodiscard]] ProtectVarSpace *getProtectVarSpace() const;
        [[nodiscard]] VarSpace *getGlobalVarSpace() const;
        [[nodiscard]] VarList *getGlobalVarlist() const;
        [[nodiscard]] Activation *getActivation() const;
        [[nodiscard]] bool checkLiteral(const std::string &element) const;
        [[nodiscard]] bool checkLiteral(const std::string &element, std::string &literaler, bool &in_protect) const;
        [[nodiscard]] EnvVarSpace *getEnvVarSpace() const;

        bool pushLiteral(const std::string &pattern, const std::string &literaler, bool in_protect);

        bool runCode();
        bool runCode(Code *code);
    };
}

#include "inter.inline.h"

#endif //AFUN_INTER_H
