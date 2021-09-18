﻿#ifndef AFUN_VAR_H_
#define AFUN_VAR_H_
#include "macro.h"

// 这些typedef可能会被下面include的文件使用
typedef struct af_VarNode af_VarNode;
typedef struct af_VarCup af_VarCup;

#include "var.h"
#include "__gc.h"

#define VAR_HASHTABLE_SIZE (8)

struct af_VarNode {
    char *id;  // 变量存储值的id为NULL
    struct af_Object *obj;
    struct af_VarNode *next;
};

struct af_Var {
    char *name;
    struct af_VarNode *vn;
    char permissions[3];  // 读-1 写-2 读且写-3 不读不写-0 [自身权限 后代权限 外部权限]
    GC_Var gc;
};

struct af_VarCup {
    struct af_Var *var;
    struct af_VarCup *next;
};

struct af_VarSpace {
    bool is_protect;  // 是否为保护变量空间
    struct af_VarCup *(var[VAR_HASHTABLE_SIZE]);
    struct af_Object *belong;  // 属主
    GC_VarSpace gc;
};

struct af_VarSpaceListNode {  // 变量链
    struct af_VarSpace *vs;
    struct af_VarSpaceListNode *next;
};

/* 变量空间创建与释放 */
af_VarSpace *makeVarSpaceByCore(af_Object *belong, af_Core *core);
void freeVarSpaceByCore(af_VarSpace *vs, af_Core *core);

/* 变量创建与释放 */
af_Var *makeVarByCore(char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_Core *core);
void freeVarByCore(af_Var *var, af_Core *core);

#endif //AFUN_VAR_H_
