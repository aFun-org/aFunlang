#ifndef AFUN__ENV_H
#define AFUN__ENV_H
#include "macro.h"
#include "tool.h"

typedef struct af_Core af_Core;
typedef struct af_Activity af_Activity;
typedef struct af_EnvVarSpace af_EnvVarSpace;
typedef struct af_EnvVar af_EnvVar;
typedef struct af_TopMsgProcess af_TopMsgProcess;
typedef struct af_LiteralDataList af_LiteralDataList;

#include "env.h"
#include "__object.h"
#include "__var.h"
#include "__code.h"
#include "__gc.h"
#include "__func.h"

#define DEFAULT_GC_COUNT_MAX (10)
#define ENV_VAR_HASH_SIZE (8)
typedef uint16_t ActivityCount;

struct af_Core {  // 解释器核心
    // GC基本信息
    struct af_ObjectData *gc_ObjectData;
    struct af_Object *gc_Object;
    struct af_Var *gc_Var;
    struct af_VarSpace *gc_VarSpace;
    size_t gc_count;  // gc量计数
    size_t gc_count_max;  // gc计数最大值
    enum GcRunTime gc_run;

    // 基本对象信息
    struct af_Object *global;  // 顶级属对象
    struct af_Object *object;  // 顶级继承对象

    // 保护空间
    bool in_init;  // 是否在初始化模式
    struct af_VarSpace *protect;  // 顶级保护变量空间

    char prefix[PREFIX_SIZE];  // 前缀
};

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

struct af_Activity {  // 活动记录器
    struct af_Activity *prev;  // 上一个活动记录器

    enum af_ActivityStatus {
        act_func = 0,
        act_arg,
        act_normal,
    } status;

    struct af_Message *msg_down;  // 被调用者向调用者传递信息
    struct af_Message *msg_up;  // 调用者向被调用者传递信息
    ActivityCount msg_up_count;  // msg_up 添加的个数

    bool run_in_func;  // 在函数变量空间内运行 (act_arg用)
    struct af_VarSpaceListNode *vsl;  // 变量空间
    struct af_VarSpaceListNode *func_var_list;  // 函数内部变量空间 (运行函数体时会设置为 主变量空间)
    struct af_VarSpaceListNode *var_list;  // 主变量空间
    ActivityCount new_vs_count;  // 需要释放的空间数

    struct af_Object *belong;  // 属对象 (belong通常为func的belong)
    struct af_Object *func;  // 函数本身

    struct af_Code *bt_top;  // 最顶层设置为NULL, 函数调用设置为block, (bt_start的上一个元素)
    struct af_Code *bt_start;  // 代码的起始位置 (block的第一个元素)
    struct af_Code *bt_next;  // 指示代码下一步要运行的位置 [总是超前当前执行的code]

    bool return_first;  // 顺序执行, 获取第一个返回结果
    struct af_Object *return_obj;  // 调用者向被调用者传递信息
    size_t process_msg_first;  // 优先处理msg而不是code

    /* 函数调用专项 */
    enum af_BlockType call_type;  // 函数调用类型
    struct af_Object *parentheses_call;  // 类前缀调用
    struct af_ArgCodeList *acl_start;
    struct af_ArgCodeList *acl_done;  // 记录当前运行的acl [总是与当前执行的acl同步] [acl代码执行完成后需要把结果写入acl, 故不超前]
    struct af_FuncInfo *fi;
    struct af_FuncBody *body_next;  // 下一个需要执行的body [总是超前当前执行的body]
    void *mark;  // 标记 [完全由API管理, 不随activity释放]
    struct af_VarSpaceListNode *macro_vsl;  // 宏函数执行的vsl
    ActivityCount macro_vs_count;

    /* 字面量专项 */
    bool is_literal;  // 处于字面量运算 意味着函数调用结束后会调用指定API
    struct af_LiteralDataList *ld;

    /* gc 机制 */
    bool is_gc;  // 处于gc的析构函数运行
    struct gc_DestructList *dl;
    struct gc_DestructList **pdl;  // 执行dl的最末端
    struct gc_DestructList *dl_next;  // dl执行的位置
};

typedef void TopMsgProcessFunc(af_Message *msg, bool is_gc, af_Environment *env);
NEW_DLC_SYMBOL(TopMsgProcessFunc, TopMsgProcessFunc);

struct af_TopMsgProcess {  // 顶层msg处理器
    char *type;
    DLC_SYMBOL(TopMsgProcessFunc) func;
    struct af_TopMsgProcess *next;
};

struct af_EnvVar {  // 环境变量
    char *name;
    char *data;
    struct af_EnvVar *next;
};

struct af_EnvVarSpace {  // 环境变量
    struct af_EnvVar *(var[ENV_VAR_HASH_SIZE]);
};

struct af_Environment {  // 运行环境
    struct af_Core *core;
    struct af_EnvVarSpace *esv;
    struct af_Activity *activity;
    struct af_TopMsgProcess *process;
};

/* Core 管理函数 */
af_Object *getBaseObjectFromCore(char *name, af_Core *core);

/* Activity 运行初始化函数 */
bool addTopActivity(af_Code *code, af_Environment *env);

/* 运行时Activity设置函数 (新增Activity) */
bool pushExecutionActivity(af_Code *bt, bool return_first, af_Environment *env);
bool pushFuncActivity(af_Code *bt, af_Environment *env);
void popActivity(af_Message *msg, af_Environment *env);

/* 运行时Activity设置函数 (设置Activity) */
bool pushDestructActivity(gc_DestructList *dl, af_Environment *env);
void pushGCActivity(gc_DestructList *dl, gc_DestructList **pdl, af_Environment *env);
bool pushVariableActivity(af_Code *bt, af_Object *func, af_Environment *env);
bool pushLiteralActivity(af_Code *bt, af_Object *func, af_Environment *env);
bool pushMacroFuncActivity(af_Object *func, af_Environment *env);
bool setFuncActivityToArg(af_Object *func, af_Environment *env);
bool setFuncActivityAddVar(af_Environment *env);
int setFuncActivityToNormal(af_Environment *env);

/* LiteralData 释放函数 */
void freeAllLiteralData(af_LiteralDataList *ld);

/* LiteralData 操作函数 */
void pushLiteralData(char *data, af_Environment *env);

/* 顶层消息处理器 处理函数 */
void runTopMessageProcess(bool is_gc, af_Environment *env);

#endif //AFUN__ENV_H
