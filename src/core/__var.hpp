﻿#ifndef AFUN_VAR_H_
#define AFUN_VAR_H_
#include "macro.hpp"
#include "pthread.h"

// 这些typedef可能会被下面include的文件使用
typedef struct af_VarNode af_VarNode;
typedef struct af_VarCup af_VarCup;

#include "var.hpp"
#include "__gc.hpp"

#define VAR_HASHTABLE_SIZE (8)

struct af_VarNode {
    char *id;  // 变量存储值的id为nullptr
    struct af_Object *obj;
    struct af_VarNode *next;
};

struct af_Var {
    char *name;
    struct af_VarNode *vn;
    char permissions[3];  // 读-1 写-2 读且写-3 不读不写-0 [自身权限 后代权限 外部权限]
    pthread_rwlock_t lock;  // 保护上面三个字段

    GC_Var gc;
};

struct af_VarCup {
    struct af_Var *var;
    struct af_VarCup *next;
};

struct af_VarSpace {
    bool is_protect;  // 是否为保护变量空间 (对VarSpaceList的操作都跳过保护空间)
    struct af_VarCup *(var[VAR_HASHTABLE_SIZE]);
    struct af_Object *belong;  // 属主
    char permissions[3];  // 可定义（2），可删除（1） [自身权限 后代权限 外部权限]
    pthread_rwlock_t lock;  // 控制 除gc外其他字段的访问

    GC_VarSpace gc;  // 仅能由 gc 机制访问
};

struct af_VarList {  // 变量链
    struct af_VarSpace *vs;
    struct af_VarList *next;
};


/* 变量空间创建与释放 */
AFUN_CORE_NO_EXPORT void freeVarSpace(af_VarSpace *vs, af_Environment *env);

/* 变量创建与释放 */
AFUN_CORE_NO_EXPORT void freeVar(af_Var *var, af_Environment *env);

#endif //AFUN_VAR_H_