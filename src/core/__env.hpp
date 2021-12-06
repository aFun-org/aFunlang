#ifndef AFUN_ENV_H_
#define AFUN_ENV_H_
#include "tool.hpp"
#include "pthread.h"

typedef struct af_Activity af_Activity;
typedef struct af_ActivityTrackBack af_ActivityTrackBack;
typedef struct af_EnvVarSpace af_EnvVarSpace;
typedef struct af_EnvVar af_EnvVar;
typedef struct af_TopMsgProcess af_TopMsgProcess;
typedef struct af_Guardian af_Guardian;
typedef struct af_LiteralDataList af_LiteralDataList;
typedef struct af_LiteralRegex af_LiteralRegex;
typedef struct af_ErrorBacktracking af_ErrorBacktracking;
typedef struct af_EnvironmentList af_EnvironmentList;

#include "env.hpp"
#include "__object.hpp"
#include "__var.hpp"
#include "__code.hpp"
#include "__gc.hpp"
#include "__func.hpp"
#include "__monitor.hpp"
#include "regex.h"

#define DEFAULT_GC_COUNT_MAX (50)
#define ENV_VAR_HASH_SIZE (8)
typedef uint16_t ActivityCount;

struct af_Message {
    char *type;  // 消息类型
    void *msg;  // 信息内容
    size_t size;
    struct af_Message *next;
};

struct af_LiteralDataList {
    char *literal_data;
    struct af_LiteralDataList *next;
};

struct af_GuardianList {
    struct af_Object *obj;
    struct af_Object *func;
    struct af_GuardianList *next;
};

enum af_ActivityType {
    act_top = 0,  /* 顶层 永远存在第一层 */
    act_func,  /* 函数调用 */
    act_top_import,  /* 导入 运算结束后global进入msg反 */
    act_guardian,  /* 守护器 */
};

enum af_ActivityStatus {
    act_func_get = 0,
    act_func_arg,
    act_func_normal,
};

struct af_Activity {  // 活动记录器
    struct af_Activity *prev;  // 上一个活动记录器

    enum af_ActivityType type;

    /* gc锁, 保护belong, run_varlist, func, return_obj, parentheses_call, macro_varlist func_varlist out_varlist gl pgl*/
    pthread_mutex_t *gc_lock;  // gc线程读时上锁, 解释器所在线程读时不上锁, 写时上锁 【本质是 Env 的 gc 锁】
    struct af_Object *belong;  // 属对象 (belong通常为func的belong)

    struct af_Message *msg_down;  // 被调用者向调用者传递信息
    struct af_Message *msg_up;  // 调用者向被调用者传递信息
    ActivityCount msg_up_count;  // msg_up 添加的个数

    struct af_VarList *run_varlist;  // 主变量空间
    ActivityCount count_run_varlist;  // 需要释放的空间数

    FilePath file;
    FileLine line;

    bool is_guard;  // 当为true时将不执行守护器

    union {
        struct {  // 仅守护器使用
            struct af_GuardianList *gl;
            struct af_GuardianList **pgl;  // 执行gl的最末端
            struct af_GuardianList *gl_next;  // gl执行的位置
        };

        struct {  // gc以外的其他内容使用
            enum af_ActivityStatus status;

            struct af_Object *func;  // 函数本身

            bool run_in_func;  // 在函数变量空间内运行 (act_arg用)
            struct af_VarList *out_varlist;  // 变量空间
            ActivityCount count_out_varlist;  // 需要释放的空间数

            struct af_VarList *func_varlist;  // 函数内部变量空间 (运行函数体时会设置为 主变量空间)

            struct af_Code *bt_top;  // 最顶层设置为nullptr, 函数调用设置为block, (bt_start的上一个元素) [只在函数调用的非NORMAL期有用]
            struct af_Code *bt_start;  // 代码的起始位置 (block的第一个元素)
            struct af_Code *bt_done;  // 当前正在执行的代码
            struct af_Code *bt_next;  // 指示代码下一步要运行的位置 [总是超前当前执行的code]

            /* 返回值 */
            bool return_first;  // 顺序执行, 获取第一个返回结果
            struct af_Object *return_obj;  // 调用者向被调用者传递信息
            size_t process_msg_first;  // 优先处理msg而不是code

            /* 函数调用: 常规 */
            enum af_BlockType call_type;  // 函数调用类型
            struct af_Object *parentheses_call;  // 类前缀调用
            struct af_ArgCodeList *acl_start;
            struct af_ArgCodeList *acl_done;  // 记录当前运行的acl [总是与当前执行的acl同步] [acl代码执行完成后需要把结果写入acl, 故不超前]
            struct af_FuncInfo *fi;
            struct af_FuncBody *body_next;  // 下一个需要执行的body [总是超前当前执行的body]
            void *mark;  // 标记 [完全由API管理, 不随activity释放]

            /* 函数调用: 宏函数*/
            bool is_macro_call;  // 宏函数隐式调用
            struct af_VarList *macro_varlist;  // 宏函数执行的vsl (即调用函数时的外部VarLis)
            ActivityCount count_macro_varlist;  // 需要释放的空间数

            /* 函数调用: 析构函数 在错误回溯时使用, 是个标记*/
            bool is_gc_call;
            bool is_guard_call;

            /* 字面量 */
            bool is_literal;  // 处于字面量运算 意味着函数调用结束后会调用指定API
            struct af_LiteralDataList *ld;

            /* 变量 */
            bool is_obj_func;  // 对象函数的调用

            /* 顺序执行 */
            bool is_execution;

            /* 尾调用优化 */
            bool optimization;

            /* Import使用 */
            char *import_mark;

            struct af_ActivityTrackBack *tb;
        };
    };
};

struct af_ActivityTrackBack {
    /* 记录Activity尾调用优化的回溯信息 */
    /* af_ActivityType不用记录, 因为必定是act_func */

    FilePath file;
    FileLine line;
    enum af_ActivityStatus status;
    bool return_first;
    bool run_in_func;
    bool is_macro_call;  // 宏函数隐式调用
    bool is_gc_call;
    bool is_literal;  // 处于字面量运算 意味着函数调用结束后会调用指定API
    bool is_obj_func;  // 对象函数的调用
    bool is_execution;
    bool optimization;

    struct af_ActivityTrackBack *next;
};

typedef void TopMsgProcessFunc(af_Message *msg, bool is_top, af_Environment *env);
NEW_DLC_SYMBOL(TopMsgProcessFunc, TopMsgProcessFunc);

typedef af_GuardianList *GuardianFunc(const char *type, bool is_guard, void *data, af_Environment *env);
NEW_DLC_SYMBOL(GuardianFunc, GuardianFunc);

typedef void GuardianDestruct(const char *type, void *data, af_Environment *env);
NEW_DLC_SYMBOL(GuardianDestruct, GuardianDestruct);

struct af_TopMsgProcess {  // 顶层msg处理器
    char *type;
    DLC_SYMBOL(TopMsgProcessFunc) func;
    struct af_TopMsgProcess *next;
};

struct af_Guardian {  // 守护器
    char *type;
    void *data;
    size_t size;
    DLC_SYMBOL(GuardianFunc) func;
    DLC_SYMBOL(GuardianDestruct) destruct;
    bool always;  // 是否总是运行
    bool derive;  // 是否支持派生
    struct af_Guardian *next;
};

struct af_EnvVar {  // 环境变量
    char *name;
    char *data;
    int32_t num;  // 可以同时记录字符串和数字
    struct af_EnvVar *next;
};

struct af_EnvVarSpace {  // 环境变量
    size_t count;
    struct af_EnvVar *(var[ENV_VAR_HASH_SIZE]);
    pthread_rwlock_t lock;
};

struct af_EnvironmentList {
    size_t id;
    struct af_Environment *env;
    struct af_EnvironmentList *next;
    struct af_EnvironmentList *prev;
};

enum af_CoreStatus {
    core_creat = 0,
    core_init,  // 执行初始化程序
    core_normal,  // 正常执行
    core_normal_gc,  // 正常执行
    core_stop,  // 当前运算退出
    core_exit,  // 解释器退出
};

struct af_Environment {  // 运行环境
    af_Monitor *monitor;  // 监视器线程

    bool is_derive;  // 是否派生
    struct af_Environment *base;  // 主线程
    struct af_Environment *gc_env;  // 主线程的 gc线程

    struct af_Object *result;  // 线程执行的结果
    struct af_EnvironmentList *env_list;  // 主线程记录所有的子线程
    bool all_exit;  // 由线程自己控制, 用于通知子线程退出
    bool son_exit;  // 由线程外部控制, 命令线程结束
    pthread_mutex_t thread_lock;  // 保护 father_exit 和 son_exit 和 son_count

    enum af_CoreStatus status;
    pthread_mutex_t status_lock;

    /* 保护空间 */
    struct af_VarSpace *protect;  // 顶级保护变量空间

    /* 数据 */
    af_EnvVar *gc_count;
    af_EnvVar *gc_max;
    af_EnvVar *gc_runtime;
    af_EnvVar *prefix;
    af_EnvVar *exit_code_;  // 退出代码
    af_EnvVar *argc;  // 参数个数
    af_EnvVar *error_std;  // Error输出的位置 0-stdout 其他-stderr

    /* 基本对象信息 */
    struct af_Object *global;  // 顶级属对象

    /* 字面量基本信息 */
    af_LiteralRegex *lr;

    struct gc_Factory *gc_factory;  // 记录gc信息

    struct af_EnvVarSpace *esv;
    struct af_TopMsgProcess *process;
    struct af_Guardian *guardian;
    struct af_Activity *activity;
    pthread_mutex_t in_run;
};

struct af_LiteralRegex {
    af_Regex *rg;
    char *pattern;  // 派生 LiteralRegex 时实用
    char *func;  // 调用的函数
    bool in_protect;  // 是否在protect空间
    struct af_LiteralRegex *next;
};

struct af_ErrorBacktracking {
    FilePath file;
    FileLine line;
    char *note;  // 备注信息
    struct af_ErrorBacktracking *next;
};

struct af_ErrorInfo {
    char *error_type;
    char *error;
    struct af_ErrorBacktracking *track;
    af_Object *obj;  // 代表错误的obj对象
};

struct af_ImportInfo {
    char *mark;
    af_Object *obj;
};

/* Core 管理函数 */
AFUN_CORE_NO_EXPORT void GcCountAdd1(af_Environment *env);
AFUN_CORE_NO_EXPORT void GcCountToZero(af_Environment *env);
AFUN_CORE_NO_EXPORT enum af_CoreStatus getCoreStatus(af_Environment *env);

/* 运行时Activity设置函数 (新增Activity) */
AFUN_CORE_NO_EXPORT bool pushExecutionActivity(af_Code *bt, bool return_first, af_Environment *env);
AFUN_CORE_NO_EXPORT bool pushFuncActivity(af_Code *bt, af_Environment *env);
AFUN_CORE_NO_EXPORT void popActivity(bool is_normal, af_Message *msg, af_Environment *env);

/* 运行时Activity设置函数 (设置Activity) */
AFUN_CORE_NO_EXPORT bool pushGuadianFuncActivity(af_GuardianList *gl, af_Environment *env);
AFUN_CORE_NO_EXPORT void pushGuardianActivity(af_GuardianList *gl, af_GuardianList **pgl, af_Environment *env);
AFUN_CORE_NO_EXPORT bool pushVariableActivity(af_Code *bt, af_Object *func, af_Environment *env);
AFUN_CORE_NO_EXPORT bool pushLiteralActivity(af_Code *bt, const char *data, af_Object *func, af_Environment *env);
AFUN_CORE_NO_EXPORT bool pushMacroFuncActivity(af_Object *func, af_Environment *env);
AFUN_CORE_NO_EXPORT bool pushImportActivity(af_Code *bt, af_Object **obj, const char *mark, af_Environment *env);
AFUN_CORE_NO_EXPORT bool setFuncActivityToArg(af_Object *func, af_Environment *env);
AFUN_CORE_NO_EXPORT bool setFuncActivityAddVar(af_Environment *env);
AFUN_CORE_NO_EXPORT int setFuncActivityToNormal(af_Environment *env);
AFUN_CORE_NO_EXPORT void setArgCodeListToActivity(af_ArgCodeList *acl, af_Environment *env);

/* 运行时Activity设置函数 (设置Activity的行号和文件) */
AFUN_CORE_NO_EXPORT void setActivityBtTop(af_Code *bt_top, af_Activity *activity);
AFUN_CORE_NO_EXPORT void setActivityBtStart(af_Code *bt_start, af_Activity *activity);
AFUN_CORE_NO_EXPORT void setActivityBtNext(af_Code *bt_next, af_Activity *activity);

/* LiteralData 释放函数 */
AFUN_CORE_NO_EXPORT void freeAllLiteralData(af_LiteralDataList *ld);

/* LiteralData 相关操作 */
AFUN_CORE_NO_EXPORT void pushLiteralData(char *data, af_Environment *env);
AFUN_CORE_NO_EXPORT bool checkLiteralCode(const char *literal, char **func, bool *in_protect, af_Environment *env);

/* 顶层消息处理器 处理函数 */
AFUN_CORE_NO_EXPORT void runTopMessageProcess(bool is_top, af_Environment *env);

/* 消息 创建与释放 */
AFUN_CORE_NO_EXPORT void freeAllMessage(af_Message *msg);

/* 消息管理函数 */
AFUN_CORE_NO_EXPORT void connectMessage(af_Message **base, af_Message *msg);

/* 环境变量管理函数 */
AFUN_CORE_NO_EXPORT af_EnvVar *setEnvVarNumber_(const char *name, int32_t data, af_Environment *env);
AFUN_CORE_NO_EXPORT af_EnvVar *setEnvVarData_(const char *name, const char *data, af_Environment *env);

/* af_GuardianList管理函数 */
AFUN_CORE_NO_EXPORT af_GuardianList **contectGuardianList(af_GuardianList *new_gl, af_GuardianList **base);

/* EnvironmentList 管理函数 */
AFUN_CORE_NO_EXPORT bool freeEnvironmentListByEnv(af_Environment *env, af_Environment *base);
AFUN_CORE_NO_EXPORT void pushEnvironmentList(af_Environment *env, af_Environment *base);

#endif //AFUN_ENV_H_
