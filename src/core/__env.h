#ifndef AFUN__ENV_H
#define AFUN__ENV_H
#include "macro.h"

typedef struct af_Core af_Core;
typedef struct af_Activity af_Activity;

#include "env.h"
#include "__object.h"
#include "__var.h"
#include "__bytecode.h"
#include "__gc.h"


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

struct af_Activity {  // 活动记录器
    struct af_Activity *prev;  // 上一个活动记录器

    struct af_ByteCode *bt_start;  // 代码的起始位置
    struct af_ByteCode *bt;  // 指示代码运行的地方

    struct af_VarSpaceListNode *var_list;  // 变量空间
    ActivityCount new_vs_count;  // 需要释放的空间数

    struct af_Object *belong;  // 属对象
};

struct af_Environment {  // 运行环境
    struct af_Core *core;
    struct af_Activity *activity;
};

af_Object *getBaseObjectFromCore(char *name, af_Core *core);

#endif //AFUN__ENV_H
