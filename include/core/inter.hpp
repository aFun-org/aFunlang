#ifndef AFUN_INTER_HPP
#define AFUN_INTER_HPP
#include <list>
#include "tool.hpp"
#include "aFunCoreExport.h"

namespace aFuncore {
    class Inter;

    enum InterStatus {
        inter_creat = 0,
        inter_init = 1,  // 执行初始化程序
        inter_normal = 2,  // 正常执行
        inter_stop = 3,  // 当前运算退出
        inter_exit = 4,  // 解释器退出
    };
    typedef enum InterStatus InterStatus;

    typedef enum ExitFlat {
        ef_activity = 0,  // 主动退出
        ef_passive = 1,  // 被动退出
        ef_none = 2,
    } ExitFlat;

    typedef enum ExitMode {
        em_activity = ef_activity,  // 主动退出
        em_passive = ef_passive,  // 被动退出
    } ExitMode;
}

#include "env-var.hpp"
#include "code.hpp"
#include "var.hpp"
#include "value.hpp"
#include "activation.hpp"

namespace aFuncore {
    class Inter {
        /* 解释器原信息记录 */
        friend class Object;
        friend class Var;
        friend class VarSpace;

        pthread_mutex_t status_lock;  // status 可能被外部使用, 因此需要用锁保护
        InterStatus status;

        /* GC 记录器 */
        struct GcRecord {
            Object *obj;
            Var *var;
            VarSpace *varspace;
        } *gc;

        /* 运行相关 */
        ProtectVarSpace *protect;  // 保护变量空间
        VarSpace *global;  // 全局变量空间
        VarList *global_varlist;  // global + protect
        Activation *activation;  // 活动记录

        struct LiteralRegex {
            Regex *rg;
            std::string pattern;  // 派生 LiteralRegex 时使用
            char *func;  // 调用的函数
            bool in_protect;  // 是否在protect空间
        };
        std::list<LiteralRegex *> *literal;

        /* 配置信息记录器 */
        EnvVarSpace *envvar;
        EnvVarSpace::EnvVar *gc_runtime;
        EnvVarSpace::EnvVar *prefix;
        EnvVarSpace::EnvVar *exit_code;  // 退出代码
        EnvVarSpace::EnvVar *argc;  // 参数个数
        EnvVarSpace::EnvVar *error_std;  // Error输出的位置 0-stdout 其他-stderr

        /* 线程信息 */
        bool is_derive;  // 是否派生
        Inter *base;  // 主线程
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
        void enable();

        Var *findGlobalVar(const std::string &name);
        VarOperationFlat defineGlobalVar(const std::string &name, Object *data);
        VarOperationFlat defineGlobalVar(const std::string &name, Var *data);
        VarOperationFlat setGlobalVar(const std::string &name, Object *data);
        VarOperationFlat delGlobalVar(const std::string &name);
        Object *findGlobalObject(const std::string &name);

        bool runCode(Code *code);
    };
}

#endif //AFUN_INTER_HPP
