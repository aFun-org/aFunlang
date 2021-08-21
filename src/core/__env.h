﻿#ifndef AFUN__ENV_H
#define AFUN__ENV_H
#include "macro.h"

typedef struct af_Core af_Core;
typedef struct af_Activity af_Activity;
typedef struct af_EnvVarSpace af_EnvVarSpace;
typedef struct af_EnvVar af_EnvVar;

#include "env.h"
#include "__object.h"
#include "__var.h"
#include "__code.h"
#include "__gc.h"

#define ENV_VAR_HASH_SIZE (8)
typedef uint16_t ActivityCount;

struct af_Core {  // 解释器核心
    // GC基本信息
    struct af_ObjectData *gc_ObjectData;
    struct af_Object *gc_Object;
    struct af_Var *gc_Var;
    struct af_VarSpace *gc_VarSpace;

    // 基本对象信息
    struct af_Object *global;  // 顶级属对象
    struct af_Object *object;  // 顶级继承对象

    // 保护空间
    bool in_init;  // 是否在初始化模式
    struct af_VarSpace *protect;  // 顶级保护变量空间
};

struct af_Message {
    char *type;  // 消息类型
    void *msg;  // 信息内容
    size_t size;
    struct af_Message *next;
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
    char **msg_type;  // 一个包含字符串的列表, 记录了需要处理的`msg`类型的数组

    struct af_VarSpaceListNode *var_list;  // 变量空间
    ActivityCount new_vs_count;  // 需要释放的空间数

    struct af_Object *belong;  // 属对象 (belong通常为func的belong)
    struct af_Object *func;  // 函数本身

    struct af_Code *bt_top;  // 最顶层设置为NULL, 函数调用设置为block, (bt_start的上一个元素)
    struct af_Code *bt_start;  // 代码的起始位置 (block的第一个元素)
    struct af_Code *bt_next;  // 指示代码下一步要运行的位置

    bool return_first;  // 顺序执行, 获取第一个返回结果
    struct af_Message *return_msg;  // 调用者向被调用者传递信息
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
    struct af_Activity *activity;
    struct af_EnvVarSpace *esv;
};

af_Object *getBaseObjectFromCore(char *name, af_Core *core);
bool pushExecutionActivity(af_Code *bt, bool return_first, af_Environment *env);
bool pushFuncActivity(af_Code *bt, af_Environment *env);
void popActivity(af_Message *msg, af_Environment *env);
bool setFuncActivityToArg(af_Object *func, af_Environment *env);
bool setFuncActivityAddVar(af_VarSpaceListNode *vsl, bool new_vsl, bool is_protect, char **msg_type, af_Environment *env);
bool setFuncActivityToNormal(af_Code *bt, af_Environment *env);
#endif //AFUN__ENV_H